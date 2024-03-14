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

#include "display/imaging/stb.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#define COMPONENT_NAME "Image Loader"

using namespace Boiler;

ImageLoader::ImageLoader() : logger(COMPONENT_NAME)
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

ImageData ImageLoader::load(const std::string &filePath)
{
	Logger logger("Image Loader");
	assert(filePath.length() > 0);
	int width, height, channels;
	const std::string &cleanedPath = FileManager::fixSpaces(filePath);
	unsigned char *pixelData = stbi_load(filePath.c_str(), &width, &height, &channels, 0);

	if (!pixelData)
	{
		logger.error("Unable to load image: {} -- {}", filePath, stbi_failure_reason());
	}

	ImageData imageData(cleanedPath, pixelData, Size(width, height), channels);
	stbi_image_free(pixelData);

	std::string fileName = std::filesystem::path{ filePath }.filename().string();
	logger.log("{} ({}x{} {}bit)", fileName, width, height, channels * 8);

	return imageData;
}
