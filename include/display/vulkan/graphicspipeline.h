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

public:
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
