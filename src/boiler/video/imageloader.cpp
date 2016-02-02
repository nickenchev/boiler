#include <SDL_image.h>
#include "engine.h"
#include "renderer.h"
#include "imageloader.h"

#define COMPONENT_NAME "SpriteLoader"

ImageLoader::ImageLoader() : Component(COMPONENT_NAME)
{
}

const std::shared_ptr<Texture> ImageLoader::loadImage(const std::string filePath) const
{
    log("Loading image: " + filePath);
    SDL_Surface *surface = IMG_Load(filePath.c_str());

    assert(surface != nullptr);
    auto texture = Engine::getInstance().getRenderer().createTexture(filePath, Size(surface->w, surface->h), surface->pixels);

    SDL_FreeSurface(surface);

    return std::move(texture);
}
