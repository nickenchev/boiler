#include <filesystem>
#include <iostream>
#include <string>
#include "core/engine.h"
#include "display/renderer.h"
#include "display/imaging/imageloader.h"
#include "display/imaging/pngloader.h"
#include "display/imaging/jpegloader.h"
#include "display/renderer.h"
#include "util/filemanager.h"
#include <display/imaging/kritaloader.h>

#include "display/imaging/stb.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>

using namespace Boiler;

ImageLoader::ImageLoader() : logger("Image Loader")
{
}

bool endsWith(const std::string &str, const std::string &endStr)
{
	bool result = false;
	if (str.length() > endStr.length())
	{
		if (str.substr(str.length() - endStr.length()) == endStr)
		{
			result = true;
		}
	}
	return result;
}

ImageData ImageLoader::load(const std::filesystem::path &filePath)
{
	Logger logger("Image Loader");
	int width, height, channels;

	unsigned char *pixelData = nullptr;
	const std::string pathString = FileManager::decodeString(filePath.string());
	if (filePath.extension() == ".kra")
	{
		std::vector<char> kritaData = loadKritaFile(filePath.string(), "mergedimage.png");
		pixelData = stbi_load_from_memory((unsigned char *)kritaData.data(), kritaData.size(), &width, &height, &channels, 0);
	}
	else
	{
		pixelData = stbi_load(pathString.c_str(), &width, &height, &channels, 0);
	}

	if (!pixelData)
	{
		logger.error("Unable to load image: {} -- {}", filePath.string(), stbi_failure_reason());
	}

	ImageData imageData(pathString, pixelData, Size(width, height), channels);
	if (pixelData)
	{
		stbi_image_free(pixelData);
	}

	std::string fileName = std::filesystem::path{ pathString }.filename().string();
	logger.log("{} ({}x{} {}bit)", fileName, width, height, channels * 8);

	return imageData;
}
