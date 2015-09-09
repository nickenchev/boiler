#include <iostream>
#include <SDL.h>

#include "engine.h"
#include "gamepart.h"
#include "spritesheet.h"
#include "spritesheetframe.h"
#include "raycaster.h"
#include "tmxloader.h"

#include "sdl_util.h" //TODO: Meh?

using namespace std;

GamePart::GamePart(Engine *engine) : Part(engine), qtree(0, Rect(0, 0, engine->getScreenWidth(),
                                                                 engine->getScreenHeight())),
                                     textFont("data/font.fnt"), gravity(9.8f), stand(0.7f), run(0.7f),
                                     jump(0.7f), falling(0)
{
    //do some loading
    playerSheet = engine->getSpriteLoader().loadSheet("data/kof.json");
    tilesSheet = engine->getSpriteLoader().loadSheet("data/tiles.json");

    //basic player setup
    player = std::make_shared<Entity>(Rect(30, 550, 15, 31));
    player->spriteSheet = playerSheet;
    addEntity(player);

    stand.addFrame(playerSheet->getFrame("stand_01.png"));
    stand.addFrame(playerSheet->getFrame("stand_02.png"));
    stand.addFrame(playerSheet->getFrame("stand_03.png"));
    stand.addFrame(playerSheet->getFrame("stand_04.png"));
    stand.addFrame(playerSheet->getFrame("stand_05.png"));
    run.addFrame(playerSheet->getFrame("run_01.png"));
    run.addFrame(playerSheet->getFrame("run_02.png"));
    run.addFrame(playerSheet->getFrame("run_03.png"));
    run.addFrame(playerSheet->getFrame("run_04.png"));
    run.addFrame(playerSheet->getFrame("run_05.png"));
    run.addFrame(playerSheet->getFrame("run_06.png"));
    jump.addFrame(playerSheet->getFrame("jump_01.png"));
    jump.addFrame(playerSheet->getFrame("jump_02.png"));
    jump.addFrame(playerSheet->getFrame("jump_03.png"));
    jump.addFrame(playerSheet->getFrame("jump_04.png"));
    jump.addFrame(playerSheet->getFrame("jump_05.png"));
    jump.loop = false;
    falling.addFrame(playerSheet->getFrame("jump_05.png"));

    currentAnimation = &stand;

    // physics setup
    grounded = false;
    jumping = false;

    // tilemap setup
    Entity tileMap[100][100];
    tmxMap = TmxLoader::loadMap("data/test_map.tmx");

    // create the entities from the tile info
    int tileNum = 0;
    int x = 0, y = 0;
    for (int r = 0; r < tmxMap->width; ++r)
    {
        for (int c = 0; c < tmxMap->height; ++c)
        {
            int tileGid = tmxMap->layers[0]->tiles[tileNum];
            if (tileGid)
            {
                const ensoft::TmxTile *tmxTile = tmxMap->allTiles[tileGid];

                auto tile = std::make_shared<Entity>(Rect(x, y, tmxMap->tilewidth, tmxMap->tileheight));
                addEntity(tile);

                const SpriteSheetFrame *tileFrame = tilesSheet->getFrame(tmxTile->image.source);
                tile->pivot = glm::vec2(0, 0);
                tile->spriteSheet = tilesSheet;
                tile->spriteFrame = tileFrame;
            }

            x += tmxMap->tilewidth;
            tileNum++;
        }
        y += tmxMap->tileheight;
        x = 0;
    }
}

void GamePart::start()
{
}

void GamePart::handleInput()
{
    // check keyboard and modify state
    if (engine->keyState(SDLK_a))
    {
        //currentAnimation = &walkLeft;
        velocity.x = -110;
        player->flipH = false;
        if (grounded)
        {
            currentAnimation = &run;
        }
    }
    else if (engine->keyState(SDLK_d))
    {
        //currentAnimation = &walkRight;
        velocity.x = 110;
        player->flipH = true;
        if (grounded)
        {
            currentAnimation = &run;
        }
    }
    else
    {
        // prepare the animations for the next play
        run.restart();

        if (grounded)
        {
            currentAnimation = &stand;
        }
        velocity.x = 0;
    }
    if (engine->keyState(SDLK_ESCAPE))
    {
        engine->quit();
    }
    // only change the velocity if there's a jump
    if (engine->keyState(SDLK_j))
    {
        if (grounded)
        {
            currentAnimation = &jump;
            jump.restart();
            jumping = true;
            grounded = false;
            // applies negative force (jump)
            velocity.y = -250;
        }
    }
}

void GamePart::update(const float delta)
{
    // update the animations
    player->spriteFrame = (*currentAnimation).getCurrentFrame();
//    if (!currentAnimation->isFinished() || currentAnimation->loop)
    {
        currentAnimation->update(delta);
    }

    // check what the player may collide with
    qtree.clear();
    for (auto &e : entities)
    {
        qtree.insert(e);
    }
    std::vector<std::shared_ptr<Entity>> closeObjects;
    qtree.retrieve(closeObjects, player->frame);

    // apply gravity before calculating final velocity
    const float terminalVelocity = 1000;
    if (velocity.y < terminalVelocity)
    {
        velocity.y += gravity;
    }

    // get movement amount based on time delta
    glm::vec2 moveAmount = velocity * delta;

    // check for collisions in the player's movement
    float yLimit = 0;
    RayCaster rayCaster;
    for (auto e : closeObjects)
    {
        if (e != player)
        {
            float numRays = 3;

            // check bottom
            if (moveAmount.y > 0)
            {
                float rayInterval = (player->frame.size.width - 2) / (numRays - 1);
                float xOrigin = player->frame.getMinX() + 1;
                bool collision = false;
                for (int r = 0; r < numRays; ++r)
                {
                    float rayOffset = r * rayInterval;

                    glm::vec2 v0(xOrigin + rayOffset, player->frame.getMaxY());
                    glm::vec2 v1 = v0 + glm::vec2(0, moveAmount.y);
                    glm::vec2 vIntersect;
                    if (rayCaster.rayCollides(v0, v1, e->frame, vIntersect))
                    {
                        // diff is the line between v0 and point of intersection
                        glm::vec2 diff = vIntersect - v0;
                        if (diff.y >= 0)
                        {
                            moveAmount.y = diff.y;
                            velocity.y = 0;
                            grounded = true;
                            jumping = false;
                            collision = true;
                        }
                    }
                }
                // if no collision was found, entity is falling
                if (!collision)
                {
                    grounded = false;
                }
            }
            else if (moveAmount.y < 0)
            {
                float rayInterval = (player->frame.size.width - 2) / (numRays - 1);
                float xOrigin = player->frame.getMinX() + 1;
                for (int r = 0; r < numRays; ++r)
                {
                    float rayOffset = r * rayInterval;

                    glm::vec2 v0(xOrigin + rayOffset, player->frame.getMinY());
                    glm::vec2 v1 = v0 + glm::vec2(0, moveAmount.y);
                    glm::vec2 vIntersect;
                    if (rayCaster.rayCollides(v0, v1, e->frame, vIntersect))
                    {
                        // diff is the line between v0 and point of intersection
                        glm::vec2 diff = vIntersect - v0;
                        if (diff.y >= 0)
                        {
                            moveAmount.y = diff.y;
                            velocity.y = 0;
                        }
                    }
                }
            }

            if (moveAmount.x != 0)
            {
                float rayInterval = (player->frame.size.height - 2) / (numRays - 1);
                float ypos = player->frame.position.y + 1;
                float xOrigin = (moveAmount.x < 0) ? player->frame.getMinX() : player->frame.getMaxX();

                for (int r = 0; r < numRays; ++r)
                {
                    float rayOffset = r * rayInterval;
                    glm::vec2 v0(xOrigin, ypos + rayOffset);
                    glm::vec2 v1 = v0 + glm::vec2(moveAmount.x, 0);
                    glm::vec2 vIntersect;

                    if (rayCaster.rayCollides(v0, v1, e->frame, vIntersect))
                    {
                        glm::vec2 diff = vIntersect - v0;
                        if (diff.x > 0)
                        {
                            moveAmount.x = diff.x;
                        }
                        else
                        {
                            moveAmount.x = 0;
                        }
                    }
                }
            }
        }
    }

    if (!grounded && !jumping)
    {
        currentAnimation = &falling;
    }

    // change the player's position based on the allowed movement amount
    player->frame.position += moveAmount;
}
