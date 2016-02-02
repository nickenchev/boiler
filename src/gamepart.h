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
#include "platformercamera.h"
#include "playerstandstate.h"

class SpriteSheet;
class SpriteSheetFrame;

class GamePart : public Part
{
    EntityState *playerState;
    std::shared_ptr<SpriteSheet> playerSheet, tilesSheet;
    std::shared_ptr<Entity> player;
    std::unique_ptr<ensoft::Map> tmxMap;
    std::shared_ptr<Camera> camera;
    Quadtree qtree;
    BMFont textFont;

    // physics stuff
    const glm::vec2 maxSpeed;
    const float gravity;
    const float jumpForceY;
    glm::vec2 velocity;
    bool grounded, jumping;

    SpriteAnimation *currentAnimation, stand, run, jump, falling, punch;

    void updateQuadtree();

public:
    GamePart(Engine &engine);

    void start() override;
    void handleInput() override;
    void update(const float delta) override;
    void drawEntity(Entity *entity);
};

#endif /* GAMEPART_H */
