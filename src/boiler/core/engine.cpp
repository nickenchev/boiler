#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include "video/opengl.h"
#include "engine.h"
#include "video/renderer.h"
#include "part.h"
#include "entity.h"
#include "input/mouseinputlistener.h"
#include "input/mousebuttonevent.h"
#include "input/keyinputlistener.h"
#include "input/keyinputevent.h"

#define RENDERER_CLASS OpenGLRenderer

Engine::Engine() : spriteLoader(), imageLoader(), fontLoader(), baseDataPath("")
{
}

Engine &Engine::getInstance()
{
    static Engine instance;
    return instance;
}

void Engine::initialize(std::unique_ptr<Renderer> renderer, const int resWidth, const int resHeight)
{
    std::cout << "* Initializing..." << std::endl;
    assert(renderer != nullptr); // No renderer provided

    this->resWidth = resWidth;
    this->resHeight = resHeight;

    // initialization was successful
    std::cout << " - Using Renderer: " << renderer->getVersion() << std::endl;
    //baseDataPath = std::string(SDL_GetBasePath());

    // initialize basic engine stuff
    frameInterval = 1.0f / 60.0f; // 60fps
    this->renderer = std::move(renderer);
    getRenderer().initialize(getScreenSize());
}

void Engine::start(std::shared_ptr<Entity> part)
{
    //store the incoming part and start it
    this->part = part;
    part->onCreate();

    //start processing events
    run();
}

void Engine::run()
{
    double prevTime = SDL_GetTicks();
    double frameLag = 0.0f;

    running = true;
    while(running)
    {
        //get the delta time
        double currentTime = SDL_GetTicks();
        double frameDelta = (currentTime - prevTime) / 1000.0f;
        prevTime = currentTime;
        frameLag += frameDelta;

        processInput();

        while (frameLag >= frameInterval)
        {
            update(frameInterval);
            frameLag -= frameInterval;
        } 

        // pass normalized 0.0 - 1.0 so renderer knows how far between frames we are
        render(frameLag / frameInterval);
    }
}

void Engine::processInput()
{
    // poll input events
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_FINGERUP:
            {
                TouchEvent event(TouchState::UP);
                for (const TouchEventListener &listener : touchEventListeners)
                {
                    listener(event);
                }
                break;
            }
            case SDL_FINGERDOWN:
            {
                TouchEvent event(TouchState::DOWN);
                for (const TouchEventListener &listener : touchEventListeners)
                {
                    listener(event);
                }
                break;
            }
            case SDL_FINGERMOTION:
            {
                TouchMotionEvent motionEvent(event.tfinger.x, event.tfinger.y,
                                             event.tfinger.dx, event.tfinger.dy);
                for (const TouchMotionListener &listener : touchMotionListeners)
                {
                    listener(motionEvent);
                }
                break;
            }
            case SDL_KEYDOWN:
            {
                SDL_Keycode keyCode = event.key.keysym.sym;
                KeyInputEvent event(keyCode, ButtonState::DOWN);

                for (auto it = keyListeners.begin(); it != keyListeners.end(); ++it)
                {
                    auto listener = static_cast<KeyInputListener *>(*it);
                    listener->onKeyStateChanged(event);
                }
                break;
            }
            case SDL_KEYUP:
            {
                SDL_Keycode keyCode = event.key.keysym.sym;
                KeyInputEvent event(keyCode, ButtonState::UP);

                for (auto it = keyListeners.begin(); it != keyListeners.end(); ++it)
                {
                    auto listener = static_cast<KeyInputListener *>(*it);
                    listener->onKeyStateChanged(event);
                }
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
                    auto listener = static_cast<MouseInputListener *>(*it);
                    listener->onMouseButton(buttonEvent);
                }
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            {
                ButtonState state = ButtonState::DOWN;
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
                    auto listener = static_cast<MouseInputListener *>(*it);
                    listener->onMouseButton(buttonEvent);
                }
                break;
            }
            case SDL_MOUSEMOTION:
            {
                for (auto it = mouseListeners.begin(); it != mouseListeners.end(); ++it)
                {
                    auto listener = static_cast<MouseInputListener *>(*it);
                    listener->setX(event.motion.x);
                    listener->setY(event.motion.y);
                    listener->onMouseMove();
                }
                break;
            }
        }
    }
}

void Engine::addMouseListener(MouseInputListener *listener)
{
    std::cout << " > Added mouse listener" << std::endl;
    mouseListeners.push_back(listener);
}

void Engine::addKeyListener(KeyInputListener *listener)
{
    std::cout << " > Added key listener" << std::endl;
    keyListeners.push_back(listener);
}

void Engine::updateEntities(const std::vector<std::shared_ptr<Entity>> &entities)
{
    for (auto &entity : entities)
    {
        entity->update();

        // update children the child entities
        if (entity->getChildren().size() > 0)
        {
            updateEntities(entity->getChildren());
        }
    }
}

void Engine::update(const float delta)
{
    part->update();

    const auto &entities = part->getChildren();
    updateEntities(entities);
}

void Engine::render(const float delta)
{
    renderer->render();
}

Engine::~Engine()
{
    mouseListeners.clear();
    keyListeners.clear();
    SDL_Quit();
}
