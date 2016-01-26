#include <SDL_image.h>
#include "engine.h"
#include "renderer.h"
#include "imageloader.h"

#define COMPONENT_NAME "SpriteLoader"

ImageLoader::ImageLoader(Engine &engine) : Component(COMPONENT_NAME, engine)
{
}

const std::shared_ptr<Texture> ImageLoader::loadImage(const std::string filename) const
{
    log("Loading image: " + filename);
    SDL_Surface *surface = IMG_Load(filename.c_str());

    assert(surface != nullptr);
    auto texture = getEngine().getRenderer().createTexture(Size(surface->w, surface->h), surface->pixels);

    SDL_FreeSurface(surface);

    return std::move(texture);
}
