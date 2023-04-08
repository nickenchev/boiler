#include <fstream>
#include "util/filemanager.h"
#include "core/logger.h"

using namespace Boiler;

std::string FileManager::readTextFile(const std::string &filePath)
{
	std::ifstream infile(filePath);
	std::stringstream buffer;
	buffer << infile.rdbuf();
	const std::string output = buffer.str();
	infile.close();

	return output;
}

std::vector<char> FileManager::readBinaryFile(const std::string &filePath)
{
    std::ostringstream buff;
    std::ifstream inFile(filePath, std::ios::binary | std::ios::ate);

    std::vector<char> output;
    if (inFile)
    {
		std::streamsize fileSize = inFile.tellg();
        output.resize(fileSize);

        inFile.seekg(std::ios::beg);
        if (!inFile.read(output.data(), fileSize))
        {
            throw std::runtime_error("Error reading entire file");
        }

		inFile.close();
    }

	return output;
}
