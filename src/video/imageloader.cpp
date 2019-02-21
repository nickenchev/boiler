#include <SDL2/SDL_image.h>
#include "core/boiler.h"
#include "video/renderer.h"
#include "video/imageloader.h"

#define COMPONENT_NAME "Image Loader"

ImageLoader::ImageLoader() : logger(COMPONENT_NAME)
{
}

const std::shared_ptr<const Texture> ImageLoader::loadImage(const std::string filePath) const
{
    logger.log("Loading image: " + filePath);
    SDL_Surface *surface = IMG_Load(filePath.c_str());

    assert(surface != nullptr);
    auto texture = Boiler::getInstance().getRenderer().createTexture(filePath, Size(surface->w, surface->h), surface->pixels);

    SDL_FreeSurface(surface);

    return std::move(texture);
}
