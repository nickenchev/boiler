#ifndef ENGINE_H
#define ENGINE_H

#include <memory>
#include "spriteloader.h"

#define RES_WIDTH 1024
#define RES_HEIGHT 768

#define MAX_KEYS 256

class Renderer;
class Part;
class SDL_Window;

class Engine
{
    std::unique_ptr<Renderer> renderer;

    bool running = true;
    SpriteLoader spriteLoader;
    unsigned int lastTime, currentTime;
    float frameDelta, frameInterval;
    bool keys[MAX_KEYS];

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

    const Renderer &getRenderer() const { return *renderer.get(); }
    const Part *getPart() const { return part; }

    bool keyState(int keyNum) const { return keys[keyNum]; }
    SpriteLoader &getSpriteLoader() { return spriteLoader; }
    int getScreenWidth() const { return RES_WIDTH; }
    int getScreenHeight() const { return RES_HEIGHT; }
};

#endif // ENGINE_H
