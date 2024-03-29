#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <memory>
#include <filesystem>
#include "core/logger.h"
#include "display/imaging/imagedata.h"

namespace Boiler
{

class ImageLoader
{
	Logger logger;
	
public:
    ImageLoader();

	virtual ImageData readImage(const std::string &filePath) const = 0;
	static ImageData load(const std::filesystem::path &filePath);
};

}
#endif /* IMAGELOADER_H */
