#ifndef SPVSHADERPROGRAM_H
#define SPVSHADERPROGRAM_H

#include <vulkan/vulkan.h>
#include <string>
#include <vector>

#include "shaderprogram.h"
#include "core/logger.h"

namespace Boiler
{

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

	void destroy() override;
	const VkPipelineShaderStageCreateInfo &getVertStageInfo() const { return vertStageInfo; }
	const VkPipelineShaderStageCreateInfo &getFragStageInfo() const { return fragStageInfo; }
};

}

#endif /* SPVSHADERPROGRAM_H */
