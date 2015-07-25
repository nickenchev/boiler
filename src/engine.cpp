#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include "engine.h"
#include "spritesheet.h"
#include "part.h"

#define RES_WIDTH 1024
#define RES_HEIGHT 768

Engine::Engine() : spriteLoader(*this), keys{0}
{
    lastTime = currentTime = 0;
    frameDelta = 0;
}

void Engine::initialize()
{
    bool success = false;
    if (SDL_Init(SDL_INIT_VIDEO) == 0)
    {
        win = SDL_CreateWindow("Boiler", 0, 0, RES_WIDTH, RES_HEIGHT, SDL_WINDOW_OPENGL);
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

void Engine::start(Part *part)
{
    //store the incoming part and start it
    this->part = part;
    part->start();

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
    part->update(delta);
}

void Engine::render(const float delta)
{
    SDL_RenderClear(ren);

    part->render();

    SDL_RenderPresent(ren);
}

Engine::~Engine()
{
    if (win)
    {
        std::cout << "* Destroying Window" << std::endl;
        SDL_DestroyWindow(win);
    }
    if (ren)
    {
        std::cout << "* Destroying Renderer" << std::endl;
        SDL_DestroyRenderer(ren);
    }
    SDL_Quit();
}
