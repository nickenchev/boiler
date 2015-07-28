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

    // render stuff
    std::unique_ptr<ShaderProgram> program;
    unsigned int vao, vboVerts, vboTexCoords, mvpUniform;
    float texCoords[12];

public:
    GamePart(Engine *engine);
    ~GamePart();
    void start() override;
    void update(const float delta) override;
    void render() override;
};
