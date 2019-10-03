#ifndef VULKANMODEL_H
#define VULKANMODEL_H

#include "model.h"
#include "vertexdata.h"
#include "video/vulkan.h"

namespace Boiler
{

class VulkanModel : public Model
{
	const VkDevice &device;
	const VkBuffer buffer;
	const VkDeviceMemory bufferMemory;

public:
	VulkanModel(const VkDevice &device, VkBuffer buffer,
				VkDeviceMemory bufferMemory, const VertexData &data);
    virtual ~VulkanModel();
};

}
#endif /* VULKANMODEL_H */
