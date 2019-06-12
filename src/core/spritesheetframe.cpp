#include "core/spritesheetframe.h"
#include "video/opengltexture.h"

using namespace Boiler;

SpriteSheetFrame::SpriteSheetFrame(const std::shared_ptr<const Texture> sourceTexture, std::string filename, Rect sourceRect, bool rotated,
								   bool trimmed, const glm::vec2 &pivot, std::shared_ptr<TextureInfo> textureInfo)
    : sourceTexture(sourceTexture), filename(filename), sourceRect(sourceRect),
	  pivot(pivot), textureInfo(textureInfo) 
{
    this->rotated = rotated;
    this->trimmed = trimmed;
}

SpriteSheetFrame::SpriteSheetFrame(const std::shared_ptr<const Texture> sourceTexture, std::shared_ptr<TextureInfo> textureInfo)
	: sourceTexture(sourceTexture), textureInfo(textureInfo)
{
}
