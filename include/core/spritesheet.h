#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <memory>
#include <map>
#include <string>
#include "rect.h"
#include "spriteframe.h"

namespace Boiler
{

typedef unsigned int GLuint;

class Texture;

class SpriteSheet
{
    const Texture texture;
    std::string imageFile;
    Size size;
    std::map<std::string, SpriteFrame> frames;

public:
    SpriteSheet(std::string imageFile, Size size, const Texture &texture,
				std::map<std::string, SpriteFrame> &frames);
    ~SpriteSheet();

    const Texture &getTexture() const { return texture; }
    std::string getImageFile() const { return imageFile; }
    const Size &getSize() const { return size; }
    const SpriteFrame *getFrame(const std::string frameName) const;
    const std::map<std::string, SpriteFrame> &getAllFrames() const { return frames; }
    const SpriteFrame *getFirstFrame() const;
};

}

#endif // SPRITESHEET_H
