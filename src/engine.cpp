#include <SDL.h>
#include <iostream>
#include "engine.h"

Engine::Engine() : spriteLoader(*this)
{
}

Engine::~Engine()
{
    if (win)
    {
        SDL_DestroyWindow(win);
    }
    if (ren)
    {
        SDL_DestroyRenderer(ren);
    }
    SDL_Quit();
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
    while(running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_KEYDOWN:
                {
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                    {
                        running = false;
                    }
                    break;
                }
            }
        }
    }
}
