#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <memory>
#include <map>
#include <string>
#include "rect.h"
#include "spritesheetframe.h"

typedef unsigned int GLuint;

class Texture;

class SpriteSheet
{
    const std::shared_ptr<const Texture> texture;
    std::string imageFile;
    Size size;
    std::map<std::string, SpriteSheetFrame> frames;

public:
    SpriteSheet(std::string imageFile, Size size, std::shared_ptr<const Texture> texture, std::map<std::string, SpriteSheetFrame> &frames);
    ~SpriteSheet();

    const Texture &getTexture() const { return *texture.get(); }
    std::string getImageFile() const { return imageFile; }
    const Size &getSize() const { return size; }
    const SpriteSheetFrame *getFrame(const std::string frameName);
    const std::map<std::string, SpriteSheetFrame> &getAllFrames() const { return frames; }
};

#endif // SPRITESHEET_H
