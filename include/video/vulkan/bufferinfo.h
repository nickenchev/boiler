#ifndef BUFFERINFO_H
#define BUFFERINFO_H

#include "video/vulkan.h"

namespace Boiler { namespace Vulkan {
struct BufferInfo
{
	VkBuffer buffer;
	VkDeviceMemory memory;
	VkDeviceSize size;

	BufferInfo()
	{
		buffer = VK_NULL_HANDLE;
		memory = VK_NULL_HANDLE;
		size = 0;
	}

	BufferInfo(VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize size)
	{
		this->buffer = buffer;
		this->memory = memory;
		this->size = size;
	}
};

}}
#endif /* BUFFERINFO_H */
