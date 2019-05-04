#include <iostream>
#include "core/spritesheet.h"
#include "video/opengl.h"

using namespace Boiler;

SpriteSheet::SpriteSheet(std::string imageFile, Size size, std::shared_ptr<const Texture> texture, std::map<std::string, SpriteSheetFrame> &frames) : size(size), frames(frames), texture(texture)
{
    this->imageFile = imageFile;
}

const SpriteSheetFrame *SpriteSheet::getFrame(const std::string frameName) const
{
    auto itr = frames.find(frameName);
    // TODO: Add show error when frame is null
    assert(itr != frames.end());

    return &itr->second;
}

const SpriteSheetFrame *SpriteSheet::getFirstFrame() const
{
	assert(frames.size() > 0);
    return &frames.begin()->second;
}

SpriteSheet::~SpriteSheet()
{
    // clear out the VBOs
    for (auto itr = frames.begin(); itr != frames.end(); ++itr)
    {
        unsigned int texCoordsVbo = itr->second.getTexCoordsVbo();
        if (texCoordsVbo)
        {
            glDeleteBuffers(1, &texCoordsVbo);
        }
    }
}
