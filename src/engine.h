#ifndef ENGINE_H
#define ENGINE_H

#include "spriteloader.h"

class SDL_Window;
class SDL_Renderer;

class Engine
{
    SDL_Window *win = nullptr;
    SDL_Renderer *ren = nullptr;
    bool running = true;
    SpriteLoader spriteLoader;
    unsigned int lastTime, currentTime;
    float frameDelta;

    void render(const float delta);

    //scene specific stuff

public:
    Engine();
    ~Engine();

    void initialize();
    void start();
    void stop() { running = false; }

    SpriteLoader &getSpriteLoader() { return spriteLoader; }
    SDL_Window *getWindow() const { return win; }
    SDL_Renderer *getRenderer() const { return ren; }
};

#endif // ENGINE_H
