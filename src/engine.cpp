#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include "engine.h"

Engine::Engine() : spriteLoader(*this)
{
}

void Engine::initialize()
{
    bool success = false;
    if (SDL_Init(SDL_INIT_VIDEO) == 0)
    {
        win = SDL_CreateWindow("Boiler", 0, 0, 640, 480, SDL_WINDOW_OPENGL);
        if (win)
        {
            ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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
    }
}

void Engine::start()
{
    running = true;
    while(running)
    {
        render();
        
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_KEYDOWN:
                {
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                    {
                        stop();
                    }
                    break;
                }
            }
        }
    }
}

void Engine::render()
{
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
