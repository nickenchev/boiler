#include "video/vulkanmodel.h"
#include "video/vulkan.h"

using namespace Boiler;

VulkanModel::VulkanModel(const VertexData &data) : Model(data.length())
{
	VkVertexInputBindingDescription inputBind = {};
	inputBind.binding = 0;
	inputBind.stride = sizeof(glm::vec3);
	inputBind.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
}


