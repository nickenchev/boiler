#ifndef SPRITESHEETFRAME_H
#define SPRITESHEETFRAME_H

#include <memory>
#include <string>
#include "rect.h"

class SpriteSheet;

class SpriteSheetFrame
{
    std::string filename;
    Rect sourceRect;

public:
    SpriteSheetFrame(std::string filename, Rect sourceRect);
    const std::string &getFilename() const { return filename; }
    Rect getSourceRect() const { return sourceRect; }
};

#endif /* SPRITESHEETFRAME_H */
