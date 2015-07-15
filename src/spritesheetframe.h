#ifndef SPRITESHEETFRAME_H
#define SPRITESHEETFRAME_H

#include <memory>
#include <string>
#include "rect.h"

class SpriteSheet;

class SpriteSheetFrame
{
    std::string filename;
    std::unique_ptr<SpriteSheet> &spriteSheet;
    Rect sourceRect;

public:
    SpriteSheetFrame(std::string filename, std::unique_ptr<SpriteSheet> &spriteSheet, Rect sourceRect);
    const std::unique_ptr<SpriteSheet> &getSpriteSheet() const { return spriteSheet; }
    Rect getSourceRect() const { return sourceRect; }
};

#endif /* SPRITESHEETFRAME_H */
