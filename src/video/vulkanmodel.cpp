#include <stdexcept>
#include "video/vulkanmodel.h"
#include "core/logger.h"

using namespace Boiler;

VulkanModel::VulkanModel(const VkDevice &device,
						 VkBuffer vertexBuffer, VkDeviceMemory vertexMemory,
						 VkBuffer indexBuffer, VkDeviceMemory indexMemory,
						 const VertexData &data, const unsigned int descriptorId) : Model(data.vertexArray().size(), data.indexArray().size()), device(device),
																					vertexBuffer(vertexBuffer), vertexMemory(vertexMemory),
																					indexBuffer(indexBuffer), indexMemory(indexMemory), descriptorId(descriptorId)
{
}

VulkanModel::~VulkanModel()
{
	vkDestroyBuffer(device, vertexBuffer, nullptr);
	vkFreeMemory(device, vertexMemory, nullptr);

	vkDestroyBuffer(device, indexBuffer, nullptr);
	vkFreeMemory(device, indexMemory, nullptr);
}
