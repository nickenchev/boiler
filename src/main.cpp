#include <iostream>
#include <SDL.h>
#include <string>
#include "spriteloader.h"


SDL_Window *win = nullptr;
SDL_Renderer *ren = nullptr;
bool running = true;

void cleanup();
void load();
void update();
void render();

int main()
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
        std::cout << "Preferred Path: " << SDL_GetPrefPath("ensoft", "sdl_engine") << std::endl;
        std::cout << "Base Path: " << SDL_GetBasePath() << std::endl;
        load();

        while(running)
        {
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                switch(event.type)
                {
                    case SDL_KEYDOWN:
                    {
                        running = false;
                        break;
                    }
                }
            }
        }
    }

    cleanup();
    return 0;
}

void cleanup()
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

void load()
{
    std::string filename = "data/random.json";
    SpriteLoader::getInstance().loadSheet(filename);
}
