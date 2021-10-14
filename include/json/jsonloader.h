#ifndef JSONLOADER_H
#define JSONLOADER_H

#include <fstream>
#include <sstream>

namespace Boiler
{
	class JsonLoader
	{
	public:
		static std::string loadJson(const std::string &gltfPath)
		{
			std::ifstream infile(gltfPath);
			std::stringstream buffer;
			buffer << infile.rdbuf();
			const std::string jsonString = buffer.str();
			infile.close();

			return jsonString;
		}
	};
}

#endif // !JSONLOADER_H