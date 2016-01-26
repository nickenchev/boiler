#ifndef ENGINE_H
#define ENGINE_H

#include <memory>
#include <vector>
#include "spriteloader.h"
#include "imageloader.h"
#include "fontloader.h"
#include "../input/mouseinputlistener.h"

#define RES_WIDTH 800
#define RES_HEIGHT 600 

#define MAX_KEYS 256

class Renderer;
class Part;
class SDL_Window;

class Engine
{
    std::unique_ptr<Renderer> renderer;
    std::vector<std::shared_ptr<MouseInputListener>> mouseListeners;

    bool running = true;

    SpriteLoader spriteLoader;
    ImageLoader imageLoader;
    FontLoader fontLoader;
    unsigned int lastTime, currentTime;
    float frameDelta, frameInterval;
    bool keys[MAX_KEYS];

    void run();
    void update(const float delta);
    void render(const float delta);

    std::shared_ptr<Part> part;

public:
    Engine();
    ~Engine();

    void initialize();
    void start(std::shared_ptr<Part> part);
    void quit() { running = false; }

    Renderer &getRenderer() const { return *renderer.get(); }
    std::shared_ptr<Part> getPart() const { return part; }

    bool keyState(int keyNum) const { return keys[keyNum]; }
    const SpriteLoader &getSpriteLoader() const { return spriteLoader; }
    const ImageLoader &getImageLoader() const { return imageLoader; }
    const FontLoader &getFontLoader() const { return fontLoader; }
    int getScreenWidth() const { return RES_WIDTH; }
    int getScreenHeight() const { return RES_HEIGHT; }

    void addMouseListener(std::shared_ptr<MouseInputListener> listener);
};

#endif // ENGINE_H
