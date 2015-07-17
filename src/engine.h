#ifndef ENGINE_H
#define ENGINE_H

#include "spriteloader.h"
#include "entity.h"

class Part;
class SDL_Window;
class SDL_Renderer;

class Engine
{
    SDL_Window *win = nullptr;
    SDL_Renderer *ren = nullptr;
    bool running = true;
    SpriteLoader spriteLoader;
    unsigned int lastTime, currentTime;
    float frameDelta, frameInterval;
    bool keys[256];

    void run();
    void update(const float delta);
    void render(const float delta);

    Part *part;

public:
    Engine();
    ~Engine();

    void initialize();
    void start(Part *part);
    void quit() { running = false; }

    bool keyState(int keyNum) const { return keys[keyNum]; }
    SpriteLoader &getSpriteLoader() { return spriteLoader; }
    SDL_Window *getWindow() const { return win; }
    SDL_Renderer *getRenderer() const { return ren; }
};

#endif // ENGINE_H
