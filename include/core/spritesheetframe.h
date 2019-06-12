#ifndef SPRITESHEETFRAME_H
#define SPRITESHEETFRAME_H

#include <memory>
#include <string>
#include "rect.h"
#include "video/textureinfo.h"

namespace Boiler
{

class SpriteSheet;
class Texture;

class SpriteSheetFrame
{
    const std::shared_ptr<const Texture> sourceTexture;
    std::string filename;
    Rect sourceRect;
    bool rotated;
    bool trimmed;
    glm::vec2 pivot;
	std::shared_ptr<const TextureInfo> textureInfo;

public:
	SpriteSheetFrame(const std::shared_ptr<const Texture> sourceTexture, std::string filename, Rect sourceRect, bool rotated,
					 bool trimmed, const glm::vec2 &pivot, std::shared_ptr<TextureInfo> textureInfo);
	SpriteSheetFrame(const std::shared_ptr<const Texture> sourceTexture, std::shared_ptr<TextureInfo> textureInfo);
    
    const std::string &getFilename() const { return filename; }
    Rect getSourceRect() const { return sourceRect; }
    bool isRotated() const { return rotated; }
    bool isTrimmed() const { return trimmed; }
    const std::shared_ptr<const Texture> getSourceTexture() const { return sourceTexture; }
	const std::shared_ptr<const TextureInfo> getTextureInfo() const { return textureInfo; }
};

}

#endif /* SPRITESHEETFRAME_H */
