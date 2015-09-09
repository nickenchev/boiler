#include <vector>
#include <memory>
#include "part.h"
#include "entity.h"
#include "tile.h"
#include "tmx.h"
#include "quadtree.h"
#include "bmfont.h"
#include "spriteanimation.h"

class SpriteSheet;
class SpriteSheetFrame;

class GamePart : public Part
{
    //scene specific stuff
    std::shared_ptr<SpriteSheet> playerSheet, tilesSheet;
    int frameNum, numFrames;
    float animTime;
    float timePerFrame;

    Quadtree qtree;
    std::shared_ptr<Entity> player;
    glm::vec2 velocity;
    const float gravity;

    BMFont textFont;

    std::unique_ptr<ensoft::Map> tmxMap;
    SpriteAnimation *currentAnimation, stand;

    // render stuff
    unsigned int vao, vboVerts, mvpUniform;
    float texCoords[12];

    // physics stuff
    bool grounded;

public:
    GamePart(Engine *engine);

    void start() override;
    void handleInput() override;
    void update(const float delta) override;
    void drawEntity(Entity *entity);
};
