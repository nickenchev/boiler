#include <vector>
#include "part.h"
#include "entity.h"

class SpriteSheet;
class SpriteSheetFrame;

class GamePart : public Part
{
    //scene specific stuff
    SpriteSheet *playerSheet;
    Entity player;
    int frameNum, numFrames;
    float animTime;
    float timePerFrame;

    const SpriteSheetFrame *frame;
    std::vector<const SpriteSheetFrame*> *currentAnimation;
    std::vector<const SpriteSheetFrame*> standLeft;
    std::vector<const SpriteSheetFrame*> standRight;
    std::vector<const SpriteSheetFrame*> walkLeft;
    std::vector<const SpriteSheetFrame*> walkRight;

public:
    GamePart(Engine *engine) : Part(engine) { }
    void start() override;
    void update(const float delta) override;
    void render() override;
};
