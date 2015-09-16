#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include "opengl.h"
#include "engine.h"
#include "part.h"
#include "openglrenderer.h"
#include "entity.h"

#define RENDERER_CLASS OpenGLRenderer

Engine::Engine() : spriteLoader(*this), keys{0}
{
    lastTime = currentTime = 0;
    frameDelta = 0;
}

void Engine::initialize()
{
    bool success = false;

    // create and initialize the renderer
    renderer = std::make_unique<RENDERER_CLASS>(*this);
    
    // initialization was successful
    std::cout << " - Preferred Path: " << SDL_GetPrefPath("ensoft", "sdl_engine") << std::endl;
    std::cout << " - Base Path: " << SDL_GetBasePath() << std::endl;

    // initialize basic engine stuff
    frameInterval = 1.0f / 60.0f; // 60fps
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
    part->handleInput();
    part->update(delta);
}

void Engine::render(const float delta)
{
    renderer->render();
}

Engine::~Engine()
{
    SDL_Quit();
}
