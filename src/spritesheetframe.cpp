#include "spritesheetframe.h"

SpriteSheetFrame::SpriteSheetFrame(std::string filename, std::unique_ptr<SpriteSheet> &spriteSheet, Rect sourceRect) : filename(filename), spriteSheet(spriteSheet), sourceRect(sourceRect)
{
}
