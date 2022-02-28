#ifndef SHADERSTAGEMODULES_H
#define SHADERSTAGEMODULES_H

#include "vulkan/vulkan_core.h"

namespace Boiler { namespace Vulkan {

struct ShaderStageModules
{
	VkShaderModule vertex;
	VkShaderModule fragment;
};

}};
#endif /* SHADERSTAGEMODULES_H */
