#include <iostream>
#include "core/spritesheet.h"
#include "display/opengl/opengl.h"

using namespace Boiler;

SpriteSheet::SpriteSheet(std::string imageFile, Size size, const Texture &texture,
						 std::map<std::string, SpriteFrame> &frames)
	: size(size), texture(texture), frames(frames)
{
    this->imageFile = imageFile;
}

const SpriteFrame *SpriteSheet::getFrame(const std::string frameName) const
{
    auto itr = frames.find(frameName);
    // TODO: Add show error when frame is null
    assert(itr != frames.end());

    return &itr->second;
}

const SpriteFrame *SpriteSheet::getFirstFrame() const
{
	assert(frames.size() > 0);
    return &frames.begin()->second;
}

SpriteSheet::~SpriteSheet()
{
}
