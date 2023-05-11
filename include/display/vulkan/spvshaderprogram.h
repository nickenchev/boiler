#ifndef SPVSHADERPROGRAM_H
#define SPVSHADERPROGRAM_H

#include <string>
#include <vector>
#include "vulkan.h"

#include "../shaderprogram.h"
#include "core/logger.h"

namespace Boiler { namespace Vulkan {

class SPVShaderProgram : public ShaderProgram
{
	static const std::string SHADER_ENTRY;

	Logger logger;
	VkDevice &device;
	VkShaderModule vertexModule;
	VkShaderModule fragmentModule;
	VkPipelineShaderStageCreateInfo vertStageInfo;
	VkPipelineShaderStageCreateInfo fragStageInfo;

	VkShaderModule createShaderModule(const std::vector<char> &contents);
	
public:
	SPVShaderProgram(VkDevice &device, std::string path, std::string vertexShader, std::string fragmentShader);
	~SPVShaderProgram();

	const VkPipelineShaderStageCreateInfo &getVertStageInfo() const { return vertStageInfo; }
	const VkPipelineShaderStageCreateInfo &getFragStageInfo() const { return fragStageInfo; }
};

}
}

#endif /* SPVSHADERPROGRAM_H */
