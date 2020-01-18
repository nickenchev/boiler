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
	const VkBuffer vertexBuffer;
	const VkDeviceMemory vertexMemory;
	const VkBuffer indexBuffer;
	const VkDeviceMemory indexMemory;
	const unsigned int descriptorId;

public:
	VulkanModel(const VkDevice &device,
				VkBuffer vertexBuffer, VkDeviceMemory vertexMemory,
				VkBuffer indexBuffer, VkDeviceMemory indexMemory,
				const VertexData &data, const unsigned int descriptorId);
    virtual ~VulkanModel();

	const VkBuffer &getVertexBuffer() const
	{
		return vertexBuffer;
	}
	const VkBuffer &getIndexBuffer() const
	{
		return indexBuffer;
	}
	const auto getDescriptorId() const
	{
		return descriptorId;
	}
};

}
#endif /* VULKANMODEL_H */
