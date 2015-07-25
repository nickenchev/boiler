#include <vector>
#include <memory>
#include "part.h"
#include "entity.h"
#include "tile.h"
#include "tmx.h"

class SpriteSheet;
class SpriteSheetFrame;

class GamePart : public Part
{
    //scene specific stuff
    SpriteSheet *playerSheet, *tilesSheet;
    int frameNum, numFrames;
    float animTime;
    float timePerFrame;

    Entity player;
    std::vector<Tile> tiles;
    glm::vec2 playerVel;

    std::unique_ptr<ensoft::Map> tmxMap;
    const SpriteSheetFrame *spriteFrame;
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
