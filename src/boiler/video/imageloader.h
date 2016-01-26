#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <memory>
#include "component.h"

class Texture;

class ImageLoader : public Component
{
public:
    ImageLoader(Engine &engine);

    const std::shared_ptr<Texture> loadImage(const std::string filename) const;
};

#endif /* IMAGELOADER_H */
