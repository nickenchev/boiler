#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <memory>
#include "core/logger.h"
#include "core/component.h"

class Texture;

class ImageLoader : public Component
{
	Logger logger;
	
public:
    ImageLoader();

    const std::shared_ptr<const Texture> loadImage(const std::string filePath) const;
};

#endif /* IMAGELOADER_H */
