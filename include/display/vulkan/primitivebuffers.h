#ifndef PRIMITIVEBUFFERS_H
#define PRIMITIVEBUFFERS_H

#include "core/asset.h"

namespace Boiler { namespace Vulkan {

class PrimitiveBuffers
{
	AssetId vertexBufferId, indexBufferId;
	
public:
	PrimitiveBuffers() { }
    PrimitiveBuffers(AssetId vertexBufferId, AssetId indexBufferId)
	{
		this->vertexBufferId = vertexBufferId;
		this->indexBufferId = indexBufferId;
	}

	AssetId getVertexBufferId() const { return vertexBufferId; }
	AssetId getIndexBufferId() const { return indexBufferId; }

	void setBuffers(AssetId vertexBufferId, AssetId indexBufferId)
	{
		this->vertexBufferId = vertexBufferId;
		this->indexBufferId = indexBufferId;
	}
};

}}

#endif /* PRIMITIVEBUFFERS_H */
