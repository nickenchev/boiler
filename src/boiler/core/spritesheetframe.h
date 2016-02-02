#ifndef SPRITESHEETFRAME_H
#define SPRITESHEETFRAME_H

#include <memory>
#include <string>
#include "rect.h"
#include "opengl.h"

class SpriteSheet;
class Texture;

class SpriteSheetFrame
{
    const std::shared_ptr<Texture> sourceTexture;
    std::string filename;
    Rect sourceRect;
    bool rotated;
    bool trimmed;
    glm::vec2 pivot;
    unsigned int texCoordsVbo;

public:
    SpriteSheetFrame(const std::shared_ptr<Texture> sourceTexture, std::string filename, Rect sourceRect, bool rotated, bool trimmed, const glm::vec2 &pivot, unsigned int texCoordsVbo);
    const std::string &getFilename() const { return filename; }
    Rect getSourceRect() const { return sourceRect; }
    unsigned int getTexCoordsVbo() const { return texCoordsVbo; }
    bool isRotated() const { return rotated; }
    bool isTrimmed() const { return trimmed; }
    const std::shared_ptr<Texture> getSourceTexture() const { return sourceTexture; }
};

#endif /* SPRITESHEETFRAME_H */
