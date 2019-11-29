#include <stdexcept>
#include "video/vulkanmodel.h"
#include "core/logger.h"

using namespace Boiler;

VulkanModel::VulkanModel(const VkDevice &device,
						 VkBuffer vertexBuffer, VkDeviceMemory vertexMemory,
						 VkBuffer indexBuffer, VkDeviceMemory indexMemory,
						 const VertexData &data) : Model(data.vertexArray().size()), device(device),
												   vertexBuffer(vertexBuffer), vertexMemory(vertexMemory),
												   indexBuffer(indexBuffer), indexMemory(indexMemory)
{
}

VulkanModel::~VulkanModel()
{
	vkDestroyBuffer(device, vertexBuffer, nullptr);
	vkFreeMemory(device, vertexMemory, nullptr);

	vkDestroyBuffer(device, indexBuffer, nullptr);
	vkFreeMemory(device, indexMemory, nullptr);
}
