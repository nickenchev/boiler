#include "core/entitycomponentsystem.h"
#include "core/components/transformcomponent.h"
#include "core/matrixcache.h"
#include "physics/physicscomponent.h"
#include "physics/collidercomponent.h"
#include "display/systems/debugrendersystem.h"
#include "display/renderer.h"
#include "util/colour.h"

using namespace Boiler;

DebugRenderSystem::DebugRenderSystem(Renderer &renderer, MatrixCache &matrixCache) : System("Debug Rendering System"), matrixCache(matrixCache)
{
	expects<TransformComponent>();
	expects<ColliderComponent>();

	primitiveIds.resize(renderer.getMaxFramesInFlight());
	for (AssetId &assetId : primitiveIds)
	{
		assetId = Asset::noAsset();
	}

	Material material;
	materialId = renderer.getAssetSet().materials.add(std::move(material));
	enabled = false;
}

void DebugRenderSystem::update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, EntityComponentSystem &ecs)
{
	for (unsigned int i = 0; i < frameInfo.keyInputEvents.getCount(); ++i)
	{
		if (frameInfo.keyInputEvents[i].keyCode == KeyCode::graveAccent && frameInfo.keyInputEvents[i].state == ButtonState::UP)
		{
			enabled = !enabled;
		}
	}

	if (enabled && getEntities().size())
	{
		std::vector<MaterialGroup> matGroups(1);
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		std::array<vec4, 4> colours = {
			Colour::fromRGBA(255, 233, 0, 100),
			Colour::fromRGBA(255, 0, 233, 100),
			Colour::fromRGBA(0, 233, 255, 100),
			Colour::fromRGBA(233, 255, 133, 100)
		};

		unsigned int index = 0;
		for (const Entity &entity : getEntities())
		{
			size_t vertOffset = vertices.size();
			size_t idxOffset = indices.size();

			auto &collider = ecs.getComponentStore().retrieve<ColliderComponent>(entity);
			auto &transform = ecs.getComponentStore().retrieve<TransformComponent>(entity);

			vec3 min = matrixCache.getMatrix(entity, ecs.getComponentStore()) * vec4(collider.min, 1);
			vec3 max = matrixCache.getMatrix(entity, ecs.getComponentStore()) * vec4(collider.max, 1);

			// generate verts
			const vec4 colour = colours[index++ % colours.size()];

			// min/max x-axis
			vertices.push_back(Vertex(vec3(min.x, min.y, min.z), colour));
			vertices.push_back(Vertex(vec3(min.x, max.y, min.z), colour));
			vertices.push_back(Vertex(vec3(min.x, min.y, max.z), colour));
			vertices.push_back(Vertex(vec3(min.x, max.y, max.z), colour));

			vertices.push_back(Vertex(vec3(max.x, min.y, min.z), colour));
			vertices.push_back(Vertex(vec3(max.x, max.y, min.z), colour));
			vertices.push_back(Vertex(vec3(max.x, min.y, max.z), colour));
			vertices.push_back(Vertex(vec3(max.x, max.y, max.z), colour));

			// min/max y-axis
			vertices.push_back(Vertex(vec3(min.x, min.y, min.z), colour));
			vertices.push_back(Vertex(vec3(max.x, min.y, min.z), colour));
			vertices.push_back(Vertex(vec3(min.x, min.y, max.z), colour));
			vertices.push_back(Vertex(vec3(max.x, min.y, max.z), colour));

			vertices.push_back(Vertex(vec3(min.x, max.y, min.z), colour));
			vertices.push_back(Vertex(vec3(max.x, max.y, min.z), colour));
			vertices.push_back(Vertex(vec3(min.x, max.y, max.z), colour));
			vertices.push_back(Vertex(vec3(max.x, max.y, max.z), colour));

			// min/max z-axis
			vertices.push_back(Vertex(vec3(min.x, min.y, min.z), colour));
			vertices.push_back(Vertex(vec3(min.x, min.y, max.z), colour));
			vertices.push_back(Vertex(vec3(min.x, max.y, min.z), colour));
			vertices.push_back(Vertex(vec3(min.x, max.y, max.z), colour));

			vertices.push_back(Vertex(vec3(max.x, min.y, min.z), colour));
			vertices.push_back(Vertex(vec3(max.x, min.y, max.z), colour));
			vertices.push_back(Vertex(vec3(max.x, max.y, min.z), colour));
			vertices.push_back(Vertex(vec3(max.x, max.y, max.z), colour));

			// center crosses
			vec3 pos = transform.translation;
			float crossSize = 0.3f;
			vec4 crossColour(1, 1, 1, 1);
			vertices.push_back(Vertex(vec3(pos.x - crossSize, pos.y, pos.z), crossColour));
			vertices.push_back(Vertex(vec3(pos.x + crossSize, pos.y, pos.z), crossColour));
			vertices.push_back(Vertex(vec3(pos.x, pos.y - crossSize, pos.z), crossColour));
			vertices.push_back(Vertex(vec3(pos.x, pos.y + crossSize, pos.z), crossColour));
		}
	
		// TODO: Generate proper indices
		for (uint32_t i = 0; i < vertices.size(); ++i)
		{
			indices.push_back(i);
		}

		// generate primitive buffers and a primitive asset
		VertexData vertData(vertices, indices);
		AssetId primBuffsId = Asset::noAsset();
		if (primitiveIds[frameInfo.currentFrame] != Asset::noAsset())
		{
			// re-create primitive buffers with new data
			AssetId existingBuffersId = renderer.getAssetSet().primitives.get(primitiveIds[frameInfo.currentFrame]).bufferId;
			primBuffsId = renderer.loadPrimitive(vertData, existingBuffersId);
		}
		else
		{
			// create new primitive buffers
			primBuffsId = renderer.loadPrimitive(vertData);
			primitiveIds[frameInfo.currentFrame] = renderer.getAssetSet().primitives
				.add(Primitive(primBuffsId, std::move(vertData), vec3(0, 0, 0), vec3(0, 0, 0))); // TODO: min/max should reflect actual values in vtx buffer
		}

		matGroups[0].materialId = materialId;
		matGroups[0].primitives.push_back(MaterialGroup::PrimitiveInstance(primitiveIds[frameInfo.currentFrame],
																		   renderer.addMatrix(mat4(1)), vec3(0), indices.size(), 0, 0));

		renderer.render(renderer.getAssetSet(), frameInfo, matGroups, RenderStage::DEBUG);
	}
}
