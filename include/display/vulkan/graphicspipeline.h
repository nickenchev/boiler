#ifndef GRAPHICSPIPELINE_H
#define GRAPHICSPIPELINE_H

#include <vector>
#include <string>

#include "display/vulkan.h"

namespace Boiler { namespace Vulkan {

struct ShaderStageModules;

class GraphicsPipeline
{
	VkPipeline pipeline;
	VkPipelineLayout layout;

public:
	static void destroy(VkDevice device, const GraphicsPipeline &pipeline);

	GraphicsPipeline();
	GraphicsPipeline(VkPipeline pipeline, VkPipelineLayout layout);

	const VkPipeline &vulkanPipeline() const
	{
		return pipeline;
	}
	const VkPipelineLayout &vulkanLayout() const
	{
		return layout;
	}
};

};
};

#endif /* GRAPHICSPIPELINE_H */
