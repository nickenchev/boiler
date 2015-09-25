#ifndef GAMEPART_H
#define GAMEPART_H

#include <vector>
#include <memory>
#include "part.h"
#include "entity.h"
#include "tile.h"
#include "tmx.h"
#include "quadtree.h"
#include "bmfont.h"
#include "spriteanimation.h"
#include "camera.h"

class SpriteSheet;
class SpriteSheetFrame;

class GamePart : public Part
{
    //scene specific stuff
    std::shared_ptr<SpriteSheet> playerSheet, tilesSheet;
    std::shared_ptr<Entity> player;
    BMFont textFont;
    std::unique_ptr<ensoft::Map> tmxMap;

    Camera camera;

    // animation related
    SpriteAnimation *currentAnimation, stand, run, jump, falling, punch;

    // collision detection specific
    Quadtree qtree;

    // physics stuff
    const glm::vec2 maxSpeed;
    const float gravity;
    const float jumpForceY;
    glm::vec2 velocity;
    bool grounded, jumping;

public:
    GamePart(Engine *engine);

    void start() override;
    void handleInput() override;
    void update(const float delta) override;
    void drawEntity(Entity *entity);
};

#endif /* GAMEPART_H */
