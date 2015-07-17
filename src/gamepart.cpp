#include <iostream>
#include <SDL.h>

#include "engine.h"
#include "gamepart.h"
#include "spritesheet.h"
#include "spritesheetframe.h"

void GamePart::start()
{
    //do some loading
    std::string filename = "data/random.json";
    playerSheet = engine->getSpriteLoader().loadSheet(filename);

    //basic player setup
    player.frame.position.y = 200;
    frameNum = 1;
    numFrames = 2;
    animTime = 0;
    timePerFrame = 0.2f;

    standLeft.push_back(playerSheet->getFrame("stand_l_01.png"));
    walkLeft.push_back(playerSheet->getFrame("walk_l_01.png"));
    walkLeft.push_back(playerSheet->getFrame("walk_l_02.png"));
    walkRight.push_back(playerSheet->getFrame("walk_r_01.png"));
    walkRight.push_back(playerSheet->getFrame("walk_r_02.png"));

    currentAnimation = &walkRight;
}

void GamePart::update(const float delta)
{
    //animation stuff
    animTime += delta;
    if (animTime >= timePerFrame)
    {
        ++frameNum;
        if (frameNum > numFrames)
        {
            frameNum = 1;
        }
        animTime = 0;
    }
    frame = (*currentAnimation)[frameNum - 1];

    // check keyboard and modify state
    if (engine->keyState(SDLK_a))
    {
        currentAnimation = &walkLeft;
        Vector2 vel(0, 1.0f);
        player.frame.position.x -= 1;
    }
    else if (engine->keyState(SDLK_d))
    {
        currentAnimation = &walkRight;
        Vector2 vel(0, -1.0f);
        player.frame.position.x += 1;
    }
    else if (engine->keyState(SDLK_ESCAPE))
    {
        engine->quit();
    }

    std::cout << delta << std::endl;
}

void GamePart::render()
{
    SDL_Rect sourceRect;
    sourceRect.x = frame->getSourceRect().position.x;
    sourceRect.y = frame->getSourceRect().position.y;
    sourceRect.w = frame->getSourceRect().size.width;
    sourceRect.h = frame->getSourceRect().size.height;

    SDL_Rect destRect;
    destRect.x = player.frame.position.x;
    destRect.y = player.frame.position.y;
    destRect.w = frame->getSourceRect().size.width;
    destRect.h = frame->getSourceRect().size.height; 

    SDL_RenderCopy(engine->getRenderer(), playerSheet->getTexture(), &sourceRect, &destRect);
}
