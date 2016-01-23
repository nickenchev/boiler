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

GamePart::GamePart(Engine &engine) : Part(engine), qtree(0, Rect(0, 0, engine.getScreenWidth(), engine.getScreenHeight())), textFont("data/font.fnt"), gravity(15.8f), stand(0.6f), run(0.6f), jump(0.6f), falling(0), punch(0.6f), jumpForceY(-600) 
{
    getEngine().getRenderer().setClearColor(Color3(0.8f, 0.8f, 1.0f));
    //do some loading
    playerSheet = getEngine().getSpriteLoader().loadSheet("data/kof.json");
    tilesSheet = getEngine().getSpriteLoader().loadSheet("data/tiles.json");

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
    tmxMap = TmxLoader::loadMap("data/demo_map.tmx");

    // create the entities from the tile info
    int tileNum = 0;
    int x = 0, y = 0;
    for (auto layer : tmxMap->layers)
    {
        cout << "Processing Layer: " << layer->name << endl;
        auto mapLayer = std::make_shared<Entity>();

        for (int r = 0; r < tmxMap->width; ++r)
        {
            int tileWidth = tmxMap->tilewidth;
            int tileHeight = tmxMap->tileheight;
            for (int c = 0; c < tmxMap->height; ++c)
            {
                int tileGid = layer->tiles[tileNum];
                if (tileGid)
                {
                    const ensoft::TmxTile *tmxTile = tmxMap->allTiles[tileGid];

					auto tile = std::make_shared<Entity>(Rect(x, y, tmxTile->image.width, tmxTile->image.height));
                    if (layer->name == "Tile Layer 1")
                    {
                        tile->collides = true;
                    }
                    tile->frame.pivot = glm::vec2(0, 1);

                    //cout << "Looking for tile image source: " << tmxTile->image.source << " (" << tile->frame.size.getWidth() << "x" << tile->frame.size.getHeight() << ")" << endl;
					const SpriteSheetFrame *tileFrame = tilesSheet->getFrame(tmxTile->image.source);
					//tile->frame.pivot = glm::vec2(0, 1);
					tile->spriteSheet = tilesSheet;
					tile->spriteFrame = tileFrame;

                    mapLayer->addChild(tile);
                }
                x += tileWidth;
                tileNum++;
            }
            y += tileHeight;
            x = 0;
        }
        x = 0;
        y = 0;
        tileNum = 0;
        addEntity(mapLayer);
    }

    //basic player setup
    player = std::make_shared<Entity>(Rect(30, -50, 62, 102));
    player->spriteSheet = playerSheet;
    addEntity(player);

    // setup the camera
    int mapWidth = tmxMap->width * tmxMap->tilewidth;
    int mapHeight = tmxMap->height * tmxMap->tileheight;
    int camWidth = getEngine().getScreenWidth() / getEngine().getRenderer().getGlobalScale().x;
    int camHeight = getEngine().getScreenHeight() / getEngine().getRenderer().getGlobalScale().y;
    camera = std::make_shared<PlatformerCamera>(Rect(0, 0, camWidth, camHeight), Size(mapWidth, mapHeight));
    camera->setCentralEntity(player);
    getEngine().getRenderer().setCamera(camera);
}

void GamePart::start()
{
}

void GamePart::handleInput()
{
    float horizontalVelocity = 250.0f;
    // check keyboard and modify state
    if (getEngine().keyState(SDLK_a))
    {
        //currentAnimation = &walkLeft;
        velocity.x = -horizontalVelocity;
        player->flipH = false;
    }
    else if (getEngine().keyState(SDLK_d))
    {
        //currentAnimation = &walkRight;
        velocity.x = horizontalVelocity;
        player->flipH = true;
    }
    else
    {
        // prepare the animations for the next play
        run.restart();
        velocity.x = 0;
    }
    if (getEngine().keyState(SDLK_ESCAPE))
    {
        getEngine().quit();
    }
    // only change the velocity if there's a jump
    if (getEngine().keyState(SDLK_j))
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
    else if (getEngine().keyState(SDLK_k))
    {
        if (punch.isFinished())
        {
            punch.restart();
        }
        currentAnimation = &punch;
    }
}

void GamePart::updateQuadtree()
{
    qtree.clear();
    for (auto &e : entities)
    {
        for (auto c : e->getChildren())
        {
            if (c->collides)
            {
                qtree.insert(c);
            }
        }
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
    updateQuadtree();
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
    if (moveAmount.x < 0)
    {
        for (auto e : closeObjects)
        {
            if (e != player)
            {
                bool left = (velocity.x < 0) ? true : false;
                glm::vec2 diff;
                if (rayCaster.detectHorizontal(player->frame, e->frame, moveAmount, numRays, left, diff))
                {
                    //e->scale = glm::vec2(0.5f, 0.5f);
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

    // update the player position and camera accordingly
    player->frame.position += ray;
    camera->update(ray);
}
