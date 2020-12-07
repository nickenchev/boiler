#ifndef GRAPHICSPIPELINE_H
#define GRAPHICSPIPELINE_H

#include <vector>
#include <string>

#include "vulkan/vulkan_core.h"

namespace Boiler { namespace Vulkan {

struct ShaderStageModules;

class GraphicsPipeline
{
	static const std::string SHADER_ENTRY;

	VkPipeline pipeline;

public:
	static GraphicsPipeline create(VkDevice device, VkRenderPass renderPass, VkPipelineLayout pipelineLayout, VkExtent2D swapChainExtent,
								   const VkVertexInputBindingDescription *inputBind, const std::vector<VkVertexInputAttributeDescription> *attrDescs,
								   const int attachmentCount, const ShaderStageModules &shaderModules, int subpassIndex, VkCullModeFlags cullMode, bool flipViewport = false);

	static void destroy(VkDevice device, const GraphicsPipeline &pipeline);

	GraphicsPipeline();
	GraphicsPipeline(VkPipeline pipeline);

	const VkPipeline &vulkanPipeline() const
	{
		return pipeline;
	}
};

};
};

#endif /* GRAPHICSPIPELINE_H */
