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
	std::vector<MaterialGroup> matGroups(1);
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	std::array<vec4, 4> colours = {
		Colour::fromRGBA(255, 233, 0, 255),
		Colour::fromRGBA(255, 0, 233, 255),
		Colour::fromRGBA(0, 233, 255, 255),
		Colour::fromRGBA(233, 255, 133, 255)
	};

	unsigned int index = 0;
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
		const vec4 colour = colours[index++ % colours.size()];

		// horizontal lines x-axis
		vertices.push_back(Vertex(vec3(min.x, min.y, min.z), colour));
		vertices.push_back(Vertex(vec3(max.x, min.y, min.z), colour));
		vertices.push_back(Vertex(vec3(min.x, max.y, min.z), colour));
		vertices.push_back(Vertex(vec3(max.x, max.y, min.z), colour));

		vertices.push_back(Vertex(vec3(min.x, min.y, max.z), colour));
		vertices.push_back(Vertex(vec3(max.x, min.y, max.z), colour));
		vertices.push_back(Vertex(vec3(min.x, max.y, max.z), colour));
		vertices.push_back(Vertex(vec3(max.x, max.y, max.z), colour));

		// vertical y-axis
		vertices.push_back(Vertex(vec3(min.x, min.y, min.z), colour));
		vertices.push_back(Vertex(vec3(min.x, max.y, min.z), colour));
		vertices.push_back(Vertex(vec3(max.x, min.y, min.z), colour));
		vertices.push_back(Vertex(vec3(max.x, max.y, min.z), colour));

		vertices.push_back(Vertex(vec3(min.x, min.y, max.z), colour));
		vertices.push_back(Vertex(vec3(min.x, max.y, max.z), colour));
		vertices.push_back(Vertex(vec3(max.x, min.y, max.z), colour));
		vertices.push_back(Vertex(vec3(max.x, max.y, max.z), colour));

		// depth lines z-axis
		vertices.push_back(Vertex(vec3(min.x, min.y, min.z), colour));
		vertices.push_back(Vertex(vec3(min.x, min.y, max.z), colour));
		vertices.push_back(Vertex(vec3(max.x, min.y, min.z), colour));
		vertices.push_back(Vertex(vec3(max.x, min.y, max.z), colour));

		vertices.push_back(Vertex(vec3(min.x, max.y, min.z), colour));
		vertices.push_back(Vertex(vec3(min.x, max.y, max.z), colour));
		vertices.push_back(Vertex(vec3(max.x, max.y, min.z), colour));
		vertices.push_back(Vertex(vec3(max.x, max.y, max.z), colour));
	}
	
	// TODO: Generate proper indices
	for (uint32_t i = 0; i < vertices.size(); ++i)
	{
		indices.push_back(i);
	}

	matGroups[0].materialId = materialId;
	matGroups[0].primitives.push_back(MaterialGroup::PrimitiveInstance(Asset::NO_ASSET, renderer.addMatrix(mat4(1)), vec3(0), indices.size() / 2, 0, 0));

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
