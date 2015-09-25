#include <iostream>
#include <SDL.h>

#include "engine.h"
#include "renderer.h"
#include "gamepart.h"
#include "spritesheet.h"
#include "spritesheetframe.h"
#include "raycaster.h"
#include "tmxloader.h"

#include "sdl_util.h" //TODO: Meh?

using namespace std;

GamePart::GamePart(Engine *engine) : Part(engine), qtree(0, Rect(0, 0, engine->getScreenWidth(), engine->getScreenHeight())), textFont("data/font.fnt"), gravity(9.8f), stand(0.6f), run(0.6f), jump(0.6f), falling(0), punch(0.6f), maxSpeed(10, 10), jumpForceY(-250), camera(Rect(0, 0, 640, 480))
{
    //do some loading
    playerSheet = engine->getSpriteLoader().loadSheet("data/kof.json");
    tilesSheet = engine->getSpriteLoader().loadSheet("data/tiles.json");

    //basic player setup
    player = std::make_shared<Entity>(Rect(30, -50, 29, 51));
    player->spriteSheet = playerSheet;
    addEntity(player);

    // setup the camera
    camera.frame.position = player->frame.position;
    getEngine()->getRenderer().setCamera(&camera);

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
    punch.addFrame(playerSheet->getFrame("punch_01.png"));
    punch.addFrame(playerSheet->getFrame("punch_02.png"));
    punch.addFrame(playerSheet->getFrame("punch_03.png"));
    punch.addFrame(playerSheet->getFrame("punch_04.png"));
    punch.addFrame(playerSheet->getFrame("punch_05.png"));
    punch.loop = false;

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
    }
    else if (engine->keyState(SDLK_d))
    {
        //currentAnimation = &walkRight;
        velocity.x = 110;
        player->flipH = true;
    }
    else
    {
        // prepare the animations for the next play
        run.restart();
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
            velocity.y += jumpForceY;
        }
    }
    else if (engine->keyState(SDLK_k))
    {
        if (punch.isFinished())
        {
            punch.restart();
        }
        currentAnimation = &punch;
    }
}

void GamePart::update(const float delta)
{
    // update the animations
    player->spriteFrame = (*currentAnimation).getCurrentFrame();
    if (!currentAnimation->isFinished() || currentAnimation->loop)
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
    RayCaster rayCaster;
    float numRays = 5;
    glm::vec2 ray(moveAmount);

    // check horizontal
    if (moveAmount.x != 0)
    {
        for (auto e : closeObjects)
        {
            if (e != player)
            {
                bool left = (velocity.x < 0) ? true : false;
                glm::vec2 diff;
                if (rayCaster.detectHorizontal(player->frame, e->frame, moveAmount, numRays, left, diff))
                {
                    e->scale = glm::vec2(0.5f, 0.5f);
                    ray.x = diff.x;
                    velocity.x = 0;
                }
            }
        }
    }
    if (moveAmount.y > 0)
    {
        for (auto e : closeObjects)
        {
            if (e != player)
            {
                glm::vec2 diff;
                if (rayCaster.detectVertical(player->frame, e->frame, moveAmount, numRays, false, diff))
                {
                    ray.y = diff.y;
                    velocity.y = 0;
                    grounded = true;
                    jumping = false;
                }
                else if (!grounded)
                {
                    grounded = false;
                }
            }
        }
    }
    else if (moveAmount.y < 0)
    {
        for (auto e : closeObjects)
        {
            if (e != player)
            {
                glm::vec2 diff;
                if (rayCaster.detectVertical(player->frame, e->frame, moveAmount, numRays, true, diff))
                {
                    ray.y = diff.y;
                    velocity.y = 0;
                }
            }
        }
    }

    // if the player is falling at a certain velocity, change animation
    if (!grounded && !jumping && velocity.y > 100)
    {
        currentAnimation = &falling;
    }
    else if (!jumping)
    {
        if (ray.x > 0)
        {
            currentAnimation = &run;
        }
        else if (ray.x < 0)
        {
            currentAnimation = &run;
        }
        else
        {
            currentAnimation = &stand;
        }
    }

    // change the player's position based on the allowed movement amount
    player->frame.position += ray;
    camera.frame.position += ray;
}
