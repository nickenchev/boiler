#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include "opengl.h"
#include "engine.h"
#include "part.h"
#include "openglrenderer.h"
#include "entity.h"
#include "../input/mousebuttonevent.h"

#define RENDERER_CLASS OpenGLRenderer

Engine::Engine() : spriteLoader(), imageLoader(), fontLoader(), keys{0}
{
    lastTime = currentTime = 0;
    frameDelta = 0;
}

Engine &Engine::getInstance()
{
    static Engine instance;
    return instance;
}

void Engine::initialize(const int resWidth, const int resHeight)
{
    this->resWidth = resWidth;
    this->resHeight = resHeight;
    // create and initialize the renderer
    renderer = std::make_unique<RENDERER_CLASS>();

    // initialization was successful
    std::cout << " - Preferred Path: " << SDL_GetPrefPath("ensoft", "sdl_engine") << std::endl;
    std::cout << " - Base Path: " << SDL_GetBasePath() << std::endl;

    // initialize basic engine stuff
    frameInterval = 1.0f / 60.0f; // 60fps
}

void Engine::start(std::shared_ptr<Part> part)
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
            // poll input events
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
                    case SDL_MOUSEBUTTONUP:
                    {
                        ButtonState state = ButtonState::UP;
                        MouseButton button;
                        if (event.button.button == SDL_BUTTON_LEFT)
                        {
                            button = MouseButton::LEFT;
                        }
                        else if (event.button.button == SDL_BUTTON_MIDDLE)
                        {
                            button = MouseButton::MIDDLE;
                        }
                        else if (event.button.button == SDL_BUTTON_RIGHT)
                        {
                            button = MouseButton::RIGHT;
                        }
                        MouseButtonEvent buttonEvent(button, state);

                        for (auto it = mouseListeners.begin(); it != mouseListeners.end(); ++it)
                        {
                            std::shared_ptr<MouseInputListener> listener = static_cast<std::shared_ptr<MouseInputListener>>(*it);
                            listener->onMouseButton(buttonEvent);
                        }
                        break;
                    }
                    case SDL_MOUSEBUTTONDOWN:
                    {
                        for (auto it = mouseListeners.begin(); it != mouseListeners.end(); ++it)
                        {
                            std::shared_ptr<MouseInputListener> listener = static_cast<std::shared_ptr<MouseInputListener>>(*it);
                            listener->setX(event.motion.x);
                            listener->setY(event.motion.y);
                            listener->onMouseMove();
                        }
                        break;
                    }
                    case SDL_MOUSEMOTION:
                    {
                        for (auto it = mouseListeners.begin(); it != mouseListeners.end(); ++it)
                        {
                            std::shared_ptr<MouseInputListener> listener = static_cast<std::shared_ptr<MouseInputListener>>(*it);
                            listener->setX(event.motion.x);
                            listener->setY(event.motion.y);
                            listener->onMouseMove();
                        }
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

void Engine::addMouseListener(std::shared_ptr<MouseInputListener> listener)
{
    std::cout << " > Added mouse listener" << std::endl;
    mouseListeners.push_back(listener);
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
    mouseListeners.clear();
    SDL_Quit();
}
