#include "video/spvshaderprogram.h"
#include "util/filemanager.h"

using namespace Boiler;

SPVShaderProgram::SPVShaderProgram(std::string path, std::string vertexShader, std::string fragmentShader) : logger("SPIR-V")
{
	auto vertContents = FileManager::readBinaryFile(path + vertexShader);
	logger.log("Loaded " + vertexShader + " Size: " + std::to_string(vertContents.size()));
	auto fragContents = FileManager::readBinaryFile(path + fragmentShader);
	logger.log("Loaded " + fragmentShader + " Size: " + std::to_string(fragContents.size()));

}

SPVShaderProgram::~SPVShaderProgram()
{
}
