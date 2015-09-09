#include "spriteanimation.h"

SpriteAnimation::SpriteAnimation(float duration)
{
    this->duration = duration;
    numPlays = 0;
    frameNum = 1;
    animTime = 0;
    timesToPlay = 1;
    loop = true;
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
            numPlays++;
        }
        animTime = 0;
    }
}

const SpriteSheetFrame *SpriteAnimation::getCurrentFrame() const
{
    return frames[frameNum - 1];
}

void SpriteAnimation::restart()
{
    frameNum = 1;
    animTime = 0;
    numPlays = 0;
}
