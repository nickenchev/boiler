#ifndef SPRITECOMPONENT_H
#define SPRITECOMPONENT_H

#include "core/component.h"

class SpriteSheetFrame;

struct SpriteComponent : public Component
{
    const SpriteSheetFrame *spriteFrame;
};

#endif /* SPRITECOMPONENT_H */
