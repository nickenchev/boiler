#ifndef SPRITESHEET_H
#define SPRITESHEET_H

class SDL_Texture;

class SpriteSheet
{
    SDL_Texture *texture;

public:
    SpriteSheet(SDL_Texture *texture);

    const SDL_Texture *getTexture() const { return texture; }
};

#endif // SPRITESHEET_H
