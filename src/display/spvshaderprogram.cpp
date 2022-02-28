#include "display/spvshaderprogram.h"
#include "util/filemanager.h"

using namespace Boiler::Vulkan;

const std::string SPVShaderProgram::SHADER_ENTRY = "main";

VkShaderModule SPVShaderProgram::createShaderModule(const std::vector<char> &contents)
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = contents.size();
	createInfo.pCode = reinterpret_cast<const uint32_t *>(contents.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("Error creating shader module");
	}
	return shaderModule;
}

SPVShaderProgram::SPVShaderProgram(VkDevice &device, std::string path,
								   std::string vertexShader, std::string fragmentShader) : logger("SPIR-V Loader"), device(device),
																						   vertStageInfo({}), fragStageInfo({})
{
	auto vertContents = FileManager::readBinaryFile(path + vertexShader);
	logger.log("Loaded " + vertexShader + " (" + std::to_string(vertContents.size()) + " bytes)");
	auto fragContents = FileManager::readBinaryFile(path + fragmentShader);
	logger.log("Loaded " + fragmentShader + " (" + std::to_string(fragContents.size()) + " bytes)");

	vertexModule = createShaderModule(vertContents);
	logger.log("Created vertex shader module");
	fragmentModule = createShaderModule(fragContents);
	logger.log("Created fragment shader module");

	vertStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertStageInfo.module = vertexModule;
	vertStageInfo.pName = SHADER_ENTRY.c_str();

	fragStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragStageInfo.module = fragmentModule;
	fragStageInfo.pName = SHADER_ENTRY.c_str();
}

SPVShaderProgram::~SPVShaderProgram()
{
	logger.log("Destroyed shader modules");
	vkDestroyShaderModule(device, vertexModule, nullptr);
	vkDestroyShaderModule(device, fragmentModule, nullptr);
}
