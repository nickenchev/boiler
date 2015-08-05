#include <vector>
#include <memory>
#include "part.h"
#include "entity.h"
#include "tile.h"
#include "tmx.h"
#include "shaderprogram.h"

class SpriteSheet;
class SpriteSheetFrame;

class GamePart : public Part
{
    //scene specific stuff
    SpriteSheet *playerSheet, *tilesSheet;
    int frameNum, numFrames;
    float animTime;
    float timePerFrame;

    std::vector<std::unique_ptr<Entity>> entities;
    Entity *player;
    glm::vec2 velocity;
    glm::vec2 gravity;

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

    void detectCollision(Entity *objectA, Entity *objectB);

public:
    GamePart(Engine *engine);
    ~GamePart();

    void start() override;
    void update(const float delta) override;
    void render() override;
    void drawEntity(Entity *entity);
    Entity *addEntity(std::unique_ptr<Entity> entity);
};
