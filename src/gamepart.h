#include <vector>
#include <memory>
#include "part.h"
#include "entity.h"
#include "tile.h"
#include "tmx.h"
#include "shaderprogram.h"
#include "quadtree.h"
#include "bmfont.h"

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
    bool isJumping;
    glm::vec2 velocity;
    const float gravity;

    BMFont textFont;

    std::unique_ptr<ensoft::Map> tmxMap;
    const SpriteSheetFrame *spriteFrame;
    std::vector<const SpriteSheetFrame*> *currentAnimation;
    std::vector<const SpriteSheetFrame*> standLeft;
    std::vector<const SpriteSheetFrame*> standRight;
    std::vector<const SpriteSheetFrame*> walkLeft;
    std::vector<const SpriteSheetFrame*> walkRight;

    // render stuff
    std::unique_ptr<ShaderProgram> program;
    unsigned int vao, vboVerts, mvpUniform;
    float texCoords[12];

    // physics stuff
    bool jumping, onGround;

public:
    GamePart(Engine *engine);
    ~GamePart();

    void start() override;
    void handleInput() override;
    void update(const float delta) override;
    void render() override;
    void drawEntity(Entity *entity);
};
