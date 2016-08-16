#ifndef ENGINE_H
#define ENGINE_H

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include "spriteloader.h"
#include "video/imageloader.h"
#include "video/fontloader.h"
#include "input/touchmotionevent.h"
#include "input/touchevent.h"

class Entity;
class Renderer;
class MouseInputListener;
class KeyInputListener;

typedef std::function<void(const TouchMotionEvent &event)> TouchMotionListener;
typedef std::function<void(const TouchEvent &event)> TouchEventListener;

class Engine
{
    int resWidth, resHeight;
    std::string baseDataPath;
    std::unique_ptr<Renderer> renderer;

    std::vector<MouseInputListener *>mouseListeners;
    std::vector<KeyInputListener *>keyListeners;
    std::vector<TouchMotionListener> touchMotionListeners;
    std::vector<TouchEventListener> touchEventListeners;

    bool running = true;

    SpriteLoader spriteLoader;
    ImageLoader imageLoader;
    FontLoader fontLoader;
    double frameInterval;

    void run();
    void processInput();
    void update(const float delta);
    void render(const float delta);

    std::shared_ptr<Entity> part;

    // singleton
    Engine();
    ~Engine();

public:
    Engine(const Engine &) = delete;
    void operator=(const Engine &s) = delete;

    static Engine &getInstance();

    void initialize(std::unique_ptr<Renderer> renderer, const int resWidth, const int resHeight);
    void start(std::shared_ptr<Entity> part);
    void quit() { running = false; }

    Renderer &getRenderer() const { return *renderer.get(); }
    std::shared_ptr<Entity> getPart() const { return part; }
    void updateEntities(const std::vector<std::shared_ptr<Entity>> &entities);

    const SpriteLoader &getSpriteLoader() const { return spriteLoader; }
    const ImageLoader &getImageLoader() const { return imageLoader; }
    const FontLoader &getFontLoader() const { return fontLoader; }

    const Size getScreenSize() const { return Size(resWidth, resHeight); }

    void addMouseListener(MouseInputListener *listener);
    void addKeyListener(KeyInputListener *listener);
    void addTouchMotionListener(TouchMotionListener listener) { touchMotionListeners.push_back(listener); }
    void addTouchEventListener(TouchEventListener listener) { touchEventListeners.push_back(listener); }
};

#endif // ENGINE_H
