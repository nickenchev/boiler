#include <stdexcept>
#include "video/vulkanmodel.h"

using namespace Boiler;

VulkanModel::VulkanModel(const VkDevice &device, VkBuffer buffer,
						 VkDeviceMemory bufferMemory, const VertexData &data) : Model(data.length()), device(device),
																				buffer(buffer), bufferMemory(bufferMemory)
{
}

VulkanModel::~VulkanModel()
{
	vkDestroyBuffer(device, buffer, nullptr);
	vkFreeMemory(device, bufferMemory, nullptr);
}
