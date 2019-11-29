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

public:
	VulkanModel(const VkDevice &device,
				VkBuffer vertexBuffer, VkDeviceMemory vertexMemory,
				VkBuffer indexBuffer, VkDeviceMemory indexMemory,
				const VertexData &data);
    virtual ~VulkanModel();

	const VkBuffer &getVertexBuffer() const
	{
		return vertexBuffer;
	}
	const VkBuffer &getIndexBuffer() const
	{
		return indexBuffer;
	}
};

}
#endif /* VULKANMODEL_H */
