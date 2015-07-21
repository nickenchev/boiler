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
    bool rotated;
    bool trimmed;
    Vector2 pivot;

public:
    SpriteSheetFrame(std::string filename, Rect sourceRect, bool rotated, bool trimmed, const Vector2 &pivot);
    const std::string &getFilename() const { return filename; }
    Rect getSourceRect() const { return sourceRect; }
    bool isRotated() const { return rotated; }
    bool isTrimmed() const { return trimmed; }
};

#endif /* SPRITESHEETFRAME_H */
