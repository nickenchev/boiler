#ifndef SPRITEANIMATION_H
#define SPRITEANIMATION_H

#include <vector>

class SpriteSheetFrame;

class SpriteAnimation
{
    std::vector<const SpriteSheetFrame *> frames;
    int frameNum;
    float animTime, duration;

public:
    SpriteAnimation(float duration);

    void addFrame(const SpriteSheetFrame *frame) { frames.push_back(frame); } 
    void update(float delta);
    const SpriteSheetFrame *getCurrentFrame() const;
};

#endif /* SPRITEANIMATION_H */
