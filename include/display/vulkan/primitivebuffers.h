#ifndef PRIMITIVEBUFFERS_H
#define PRIMITIVEBUFFERS_H

#include "bufferinfo.h"

namespace Boiler { namespace Vulkan {

class PrimitiveBuffers
{
	BufferInfo vertexBuffer;
	BufferInfo indexBuffer;
	
public:
	PrimitiveBuffers() { }
    PrimitiveBuffers(BufferInfo vertexBuffer, BufferInfo indexBuffer)
	{
		this->vertexBuffer = vertexBuffer;
		this->indexBuffer = indexBuffer;
	}

	const BufferInfo &getVertexBuffer() const { return vertexBuffer; }
	const BufferInfo &getIndexBuffer() const { return indexBuffer; }
};

}}

#endif /* PRIMITIVEBUFFERS_H */
