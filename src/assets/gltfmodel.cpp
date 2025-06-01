#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "assets/gltfmodel.h"
#include "core/entitycomponentsystem.h"
#include "core/components/transformcomponent.h"
#include "animation/components/animationcomponent.h"
#include "core/components/rendercomponent.h"
#include "core/components/parentcomponent.h"
#include "physics/collidercomponent.h"
#include "physics/physicscomponent.h"

using namespace Boiler;

GLTFModel::GLTFModel(const std::string &filePath, const std::vector<std::vector<std::byte>> &&buffers,
                     gltf::Model &&model, ImportResult &&result)
    : filePath(filePath), buffers(buffers), model(model), result(result)
{
}

std::vector<Entity> GLTFModel::createInstance(const AssetSet &assetSet, EntityComponentSystem &ecs, const Entity &rootEntity) const
{
    // TODO: Remove array from this call to avoid memory allocation during runtime

    // grab the default scene and load the node heirarchy
    const gltf::Scene &scene = model.scenes[model.scene];

    // root entity MUST have a transform
    if (!ecs.hasComponent<TransformComponent>(rootEntity))
    {
        ecs.createComponent<TransformComponent>(rootEntity);
    }

    // used to map between node indexes (glTF) and entity system IDs
    std::vector<Entity> nodeEntities(model.nodes.size(), Entity::NO_ENTITY);

    if (scene.nodes.size() == 1)
    {
        loadNode(assetSet, ecs, nodeEntities, rootEntity, scene.nodes[0], Entity::NO_ENTITY);
    }
    else
    {
        for (auto &nodeIndex : scene.nodes)
        {
            loadNode(assetSet, ecs, nodeEntities, ecs.newEntity(model.nodes[nodeIndex].name), nodeIndex, rootEntity);
        }
    }

    // target -> entity mapping needed for animations
    ecs.createComponent<AnimationComponent>(rootEntity, nodeEntities);

    auto &animComp = ecs.getComponentStore().retrieve<AnimationComponent>(rootEntity);
    for (AnimationId animId : getImportResult().animations)
    {
        animComp.addClip(Clip(0, animId, true));
    }

    return nodeEntities;
}

Entity GLTFModel::loadNode(const AssetSet &assetSet, EntityComponentSystem &ecs, std::vector<Entity> &nodeEntities, const Entity nodeEntity, int nodeIndex, const Entity parentEntity) const
{
    if (!nodeEntities[nodeIndex] != Entity::NO_ENTITY)
    {
        nodeEntities[nodeIndex] = nodeEntity;

        // set parent entity if provided
        if (parentEntity != Entity::NO_ENTITY)
        {
            ecs.createComponent<ParentComponent>(nodeEntity, parentEntity);
        }

        const gltf::Node &node = model.nodes[nodeIndex];
        logger.log("Processing node: {}", node.name);

        // assign mesh
        if (node.mesh.has_value())
        {
            auto &render = ecs.createComponent<RenderComponent>(nodeEntity);
            render.meshId = result.meshes[node.mesh.value()];

            // generate collider component
            if (!ecs.getComponentStore().hasComponent<ColliderComponent>(nodeEntity))
            {
                const Mesh &mesh = assetSet.meshes.get(render.meshId);
                auto &collider = ecs.createComponent<ColliderComponent>(nodeEntity);
                collider.min = mesh.min;
                collider.max = mesh.max;
                logger.log("Created collision component from GLTF data");
                ecs.createComponent<PhysicsComponent>(nodeEntity);
            }
        }

        // create a transform component if one doesn't exist
        TransformComponent &transform = ecs.getComponentStore().hasComponent<TransformComponent>(nodeEntity)
            ? ecs.getComponentStore().retrieve<TransformComponent>(nodeEntity)
            : ecs.createComponent<TransformComponent>(nodeEntity, vec3(0, 0, 0));

        // decompose a matrix if available, otherwise try to load transformations directly
        if (node.matrix.has_value())
        {
            const auto &matrixArray = node.matrix.value();
            mat4 matrix = glm::make_mat4(matrixArray.data());
            vec3 scale, skew, position;
            glm::quat orientation;
            vec4 perspective;

            glm::decompose(matrix, scale, orientation, position, skew, perspective);
            transform.translation = position;
            transform.scale = scale;
            transform.orientation = orientation;
        }
        else
        {
            // otherwise load transformation directly
            if (node.scale.has_value())
            {
                transform.scale = {
                    node.scale.value()[0],
                    node.scale.value()[1],
                    node.scale.value()[2]
                };
            }
            if (node.translation.has_value())
            {
                transform.translation = {
                    node.translation.value()[0],
                    node.translation.value()[1],
                    node.translation.value()[2]
                };
            }
            if (node.rotation.has_value())
            {
                transform.orientation = quat{
                    node.rotation.value()[3],
                    node.rotation.value()[0],
                    node.rotation.value()[1],
                    node.rotation.value()[2]
                };
            }
        }

        // create child nodes, setting current node as parent
        if (node.children.size() > 0)
        {
            for (int childIndex : node.children)
            {
                loadNode(assetSet, ecs, nodeEntities, ecs.newEntity(model.nodes[childIndex].name), childIndex, nodeEntity);
            }
        }
    }
    return nodeEntity;
}

Logger GLTFModel::logger("GLTF Model");
