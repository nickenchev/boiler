#include "spriteanimation.h"

SpriteAnimation::SpriteAnimation(float duration)
{
    this->duration = duration;
    frameNum = 1;
    animTime = 0;
}

void SpriteAnimation::update(float delta)
{
    float timePerFrame = duration / frames.size();

    //animation stuff
    animTime += delta;
    if (animTime >= timePerFrame)
    {
        ++frameNum;
        if (frameNum > frames.size())
        {
            frameNum = 1;
        }
        animTime = 0;
    }
}

const SpriteSheetFrame *SpriteAnimation::getCurrentFrame() const
{
    return frames[frameNum - 1];
}
