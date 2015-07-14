#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include "engine.h"
#include "spritesheet.h"

Engine::Engine() : spriteLoader(*this)
{
    lastTime = currentTime = 0;
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
    }
}

void Engine::start()
{
    running = true;
    while(running)
    {
        lastTime = currentTime;
        currentTime = SDL_GetTicks();
        unsigned int delta = (currentTime - lastTime) / 1000;

        if (true)
        {
            render(delta);

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
}

void Engine::render(unsigned int delta)
{
    std::cout << delta << std::endl;
    const SpriteSheet &sheet = spriteLoader.getSpriteSheets()[0];
    SDL_RenderClear(ren);
    SDL_Rect destRect;
    destRect.x = 50;
    destRect.y = 50;
    destRect.w = sheet.getSize().width;
    destRect.h = sheet.getSize().height;
    SDL_RenderCopy(ren, sheet.getTexture(), nullptr, &destRect);
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
