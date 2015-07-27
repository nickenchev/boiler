#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <memory>
#include <map>
#include <string>
#include "rect.h"
#include "spritesheetframe.h"

typedef unsigned int GLuint;

class SpriteSheet
{
    GLuint texture;
    std::string imageFile;
    Size size;
    std::map<std::string, SpriteSheetFrame> frames;

public:
    SpriteSheet(std::string imageFile, Size size, GLuint texture, std::map<std::string, SpriteSheetFrame> &frames);

    const GLuint &getTexture() const { return texture; }
    std::string getImageFile() const { return imageFile; }
    const Size &getSize() const { return size; }
    const SpriteSheetFrame *getFrame(std::string frameName) const { return &(frames.find(frameName)->second); }
};

#endif // SPRITESHEET_H
