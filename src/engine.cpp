#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include "engine.h"
#include "spritesheet.h"

Engine::Engine() : spriteLoader(*this)
{
    lastTime = currentTime = 0;
    frameDelta = 0;
}

void Engine::initialize()
{
    bool success = false;
    if (SDL_Init(SDL_INIT_VIDEO) == 0)
    {
        win = SDL_CreateWindow("Boiler", 0, 0, 640, 480, SDL_WINDOW_OPENGL);
        if (win)
        {
            ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
            if (ren)
            {
                IMG_Init(IMG_INIT_PNG);
                success = true;
            }
        }
    }

    if (!success)
    {
        std::cout << "Error Initializing SDL: " << SDL_GetError() << std::endl;
    }
    else
    {
        std::cout << " - Preferred Path: " << SDL_GetPrefPath("ensoft", "sdl_engine") << std::endl;
        std::cout << " - Base Path: " << SDL_GetBasePath() << std::endl;

        //initialize basic engine stuff
        frameInterval = 1.0f / 60.0f; // 60fps
    }
}

void Engine::start()
{
    //do some loading
    std::string filename = "data/random.json";
    playerSheet = getSpriteLoader().loadSheet(filename);

    //basic player setup
    player.frame.position.y = 200;
    frameNum = 1;
    numFrames = 2;
    animTime = 0;
    timePerFrame = 0.2f;

    walkLeft.push_back(playerSheet->getFrame("walk_l_01.png"));
    walkLeft.push_back(playerSheet->getFrame("walk_l_02.png"));
    walkRight.push_back(playerSheet->getFrame("walk_r_01.png"));
    walkRight.push_back(playerSheet->getFrame("walk_r_02.png"));

    currentAnimation = &walkRight;

    //start processing events
    run();
}

void Engine::run()
{
    running = true;
    while(running)
    {
        //get the delta time
        currentTime = SDL_GetTicks();
        frameDelta += (currentTime - lastTime) / 1000.0f;

        //if its time to process a frame, do it here 
        if (frameDelta >= frameInterval)
        {
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                switch(event.type)
                {
                    case SDL_KEYDOWN:
                    {
                        SDL_Keycode keyCode = event.key.keysym.sym;
                        keys[keyCode] = true;
                        break;
                    }
                    case SDL_KEYUP:
                    {
                        SDL_Keycode keyCode = event.key.keysym.sym;
                        keys[keyCode] = false;
                        break;
                    }
                }
            }
            //update the state and render to screen
            update(frameDelta);
            render(frameDelta);

            frameDelta = 0;
        }
        lastTime = currentTime;
    }
}

void Engine::update(const float delta)
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
    if (keys[SDLK_a])
    {
        currentAnimation = &walkLeft;
        Vector2 vel(0, 1.0f);
        player.frame.position.x -= 1;
    }
    else if (keys[SDLK_d])
    {
        currentAnimation = &walkRight;
        Vector2 vel(0, -1.0f);
        player.frame.position.x += 1;
    }
    else if (keys[SDLK_ESCAPE])
    {
        stop();
    }
}

void Engine::render(const float delta)
{
    SDL_RenderClear(ren);

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

    SDL_RenderCopy(ren, playerSheet->getTexture(), &sourceRect, &destRect);
    SDL_RenderPresent(ren);
}

Engine::~Engine()
{
    if (win)
    {
        std::cout << "* Destroing Window" << std::endl;
        SDL_DestroyWindow(win);
    }
    if (ren)
    {
        std::cout << "* Destroing Renderer" << std::endl;
        SDL_DestroyRenderer(ren);
    }
    SDL_Quit();
}
