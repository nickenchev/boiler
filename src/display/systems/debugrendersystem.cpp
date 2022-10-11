#include "core/entitycomponentsystem.h"
#include "core/components/transformcomponent.h"
#include "physics/collisioncomponent.h"
#include "physics/physicscomponent.h"
#include "display/systems/debugrendersystem.h"
#include "display/renderer.h"
#include "util/colour.h"

using namespace Boiler;

DebugRenderSystem::DebugRenderSystem(Renderer &renderer) : System("Debug Rendering System")
{
	expects<TransformComponent>();
	expects<CollisionComponent>();
	expects<PhysicsComponent>();

	primitiveIds.resize(renderer.getMaxFramesInFlight());
	for (AssetId &assetId : primitiveIds)
	{
		assetId = Asset::NO_ASSET;
	}

	Material material;
	materialId = renderer.getAssetSet().materials.add(std::move(material));
}

void DebugRenderSystem::update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, EntityComponentSystem &ecs)
{
	std::vector<MaterialGroup> matGroups;
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	MaterialGroup matGroup;
	matGroup.materialId = materialId;

    for (const Entity &entity : getEntities())
	{
		size_t vertOffset = vertices.size();
		size_t idxOffset = indices.size();
        auto &collision = ecs.getComponentStore().retrieve<CollisionComponent>(entity);
        auto &transform = ecs.getComponentStore().retrieve<TransformComponent>(entity);
        auto &physics = ecs.getComponentStore().retrieve<PhysicsComponent>(entity);

		TransformComponent newTransformA = transform;
		newTransformA.setPosition(newTransformA.getPosition() + physics.velocity * frameInfo.deltaTime);

		vec3 min = vec3(newTransformA.getMatrix() * vec4(collision.min, 1));
		vec3 max = vec3(newTransformA.getMatrix() * vec4(collision.max, 1));

		// generate verts
		const vec4 colour = Colour::fromRGBA(255, 233, 0, 255);

		vertices.push_back(Vertex(vec3(min.x, min.y, min.z), colour));
		vertices.push_back(Vertex(vec3(max.x, min.y, min.z), colour));

		indices.push_back(0);
		indices.push_back(1);

		AssetId matrixId = renderer.addMatrix(mat4(1));
		matGroup.primitives.push_back(MaterialGroup::PrimitiveInstance(Asset::NO_ASSET, matrixId, vec3(0, 0, 0), 2, vertOffset, idxOffset));
	}
	matGroups.push_back(matGroup);

	// generate primitive buffers and a primitive asset
	VertexData vertData(vertices, indices);
	AssetId primBuffsId = Asset::NO_ASSET;
	if (primitiveIds[frameInfo.currentFrame] != Asset::NO_ASSET)
	{
		// re-create primitive buffers with new data
		AssetId existingBuffersId = renderer.getAssetSet().primitives.get(primitiveIds[frameInfo.currentFrame]).bufferId;
		primBuffsId = renderer.loadPrimitive(vertData, existingBuffersId);
	}
	else
	{
		// create new primitive buffers
		primBuffsId = renderer.loadPrimitive(vertData);
		primitiveIds[frameInfo.currentFrame] = renderer.getAssetSet().primitives.add(Primitive(primBuffsId, std::move(vertData), vec3(0, 0, 0), vec3(0, 0, 0))); // TODO: min/max should reflect actual values in vtx buffer
	}

	for (MaterialGroup::PrimitiveInstance &instance : matGroups[0].primitives)
	{
		instance.primitiveId = primitiveIds[frameInfo.currentFrame];
	}

	renderer.render(renderer.getAssetSet(), frameInfo, matGroups, RenderStage::DEBUG);
}
