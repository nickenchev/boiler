#ifndef SPRITEANIMATION_H
#define SPRITEANIMATION_H

#include <vector>

class SpriteSheetFrame;

class SpriteAnimation
{
    std::vector<const SpriteSheetFrame *> frames;
    int frameNum, numPlays;
    float animTime, duration;

public:
    bool loop;
    int timesToPlay;

    SpriteAnimation(float duration);

    void addFrame(const SpriteSheetFrame *frame) { frames.push_back(frame); } 
    void update(float delta);
    const SpriteSheetFrame *getCurrentFrame() const;
    void restart();
    bool isFinished() const { return numPlays < timesToPlay; }
};

#endif /* SPRITEANIMATION_H */
