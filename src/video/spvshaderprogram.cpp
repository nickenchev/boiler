#include "video/spvshaderprogram.h"
#include "util/filemanager.h"

using namespace Boiler;

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

SPVShaderProgram::SPVShaderProgram(VkDevice &device, std::string path, std::string vertexShader, std::string fragmentShader) : logger("SPIR-V"), device(device)
{
	auto vertContents = FileManager::readBinaryFile(path + vertexShader);
	logger.log("Loaded " + vertexShader + " Size: " + std::to_string(vertContents.size()));
	auto fragContents = FileManager::readBinaryFile(path + fragmentShader);
	logger.log("Loaded " + fragmentShader + " Size: " + std::to_string(fragContents.size()));

	vertexModule = createShaderModule(vertContents);
	logger.log("Created vertex shader module");
	fragmentModule = createShaderModule(fragContents);
	logger.log("Created fragment shader module");

}

void SPVShaderProgram::destroy()
{
	logger.log("Destroyed shader modules");
	vkDestroyShaderModule(device, vertexModule, nullptr);
	vkDestroyShaderModule(device, fragmentModule, nullptr);
}
