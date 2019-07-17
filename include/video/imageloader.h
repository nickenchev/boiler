#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <memory>
#include "core/logger.h"

namespace Boiler
{

class Texture;
class Renderer;

class ImageLoader
{
	Logger logger;
	const Renderer &renderer;
	
public:
    ImageLoader(const Renderer &renderer);

	const std::shared_ptr<const Texture> loadImage(const std::string &filePath) const;
};

}
#endif /* IMAGELOADER_H */
