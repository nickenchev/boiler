#include <iostream>
#include "core/spriteanimation.h"

using namespace Boiler;

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
    animTime += delta;
    if (animTime >= timePerFrame)
    {
        ++frameNum;
        if (frameNum > static_cast<unsigned int>(frames.size()))
        {
            numPlays++;
            if (!isFinished() || loop)
            {
                frameNum = 1;
            }
            else
            {
                frameNum--;
            }
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

bool SpriteAnimation::isFinished() const
{
    return numPlays >= timesToPlay;
}
