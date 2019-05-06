#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <memory>
#include "core/logger.h"

namespace Boiler
{

class Texture;

class ImageLoader
{
	Logger logger;
	
public:
    ImageLoader();

	const std::shared_ptr<const Texture> loadImage(const std::string &filePath) const;
};

}
#endif /* IMAGELOADER_H */
