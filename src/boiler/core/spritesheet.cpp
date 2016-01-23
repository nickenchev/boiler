#include "spritesheet.h"

SpriteSheet::SpriteSheet(std::string imageFile, Size size, std::shared_ptr<Texture> texture, std::map<std::string, SpriteSheetFrame> &frames) : size(size), frames(frames), texture(texture)
{
    this->imageFile = imageFile;
}

SpriteSheet::~SpriteSheet()
{
    for (const auto &frameName : frames)
    {
        GLuint vbo = getFrame(frameName.first)->getTexCoordsVbo();
        glDeleteBuffers(1, &vbo);
    }
}