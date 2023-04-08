#pragma once

#include "util/filemanager.h"

namespace Boiler
{
	class JsonLoader
	{
	public:
		static std::string loadJson(const std::string &gltfPath)
		{
			return FileManager::readTextFile(gltfPath);
		}
	};
}
