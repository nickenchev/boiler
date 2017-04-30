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
#include "input/touchtapevent.h"
#include "input/mousemotionevent.h"
#include "input/keyinputevent.h"
#include "core/entitycomponentsystem.h"

class Entity;
class Renderer;

typedef std::function<void(const TouchMotionEvent &event)> TouchMotionListener;
typedef std::function<void(const TouchTapEvent &event)> TouchTapEventListener;
typedef std::function<void(const MouseMotionEvent &event)> MouseMotionListener;
typedef std::function<void(const KeyInputEvent &event)> KeyInputListener;

class Boiler
{
	EntityComponentSystem ecs;
    int resWidth, resHeight;
    std::string baseDataPath;
    std::unique_ptr<Renderer> renderer;

    std::vector<TouchMotionListener> touchMotionListeners;
    std::vector<TouchTapEventListener> touchTapEventListeners;
    std::vector<MouseMotionListener> mouseMotionListeners;
    std::vector<KeyInputListener> keyInputListeners;

    bool running = true;

    SpriteLoader spriteLoader;
    ImageLoader imageLoader;
    FontLoader fontLoader;
    double frameInterval;

    void run();
    void processInput();
    void update(const double delta);
    void render(const double delta);

    std::shared_ptr<Entity> part;

    // singleton
    Boiler();
    ~Boiler();

public:
    Boiler(const Boiler &) = delete;
    void operator=(const Boiler &s) = delete;

    static Boiler &getInstance();

    void initialize(std::unique_ptr<Renderer> renderer, const int resWidth, const int resHeight);
    void start(std::shared_ptr<Entity> part);
    void quit() { running = false; }

	EntityComponentSystem &getEcs() { return ecs; }
    Renderer &getRenderer() const { return *renderer.get(); }
    std::shared_ptr<Entity> getPart() const { return part; }
    void updateEntities(const std::vector<std::shared_ptr<Entity>> &entities);

    const SpriteLoader &getSpriteLoader() const { return spriteLoader; }
    const ImageLoader &getImageLoader() const { return imageLoader; }
    const FontLoader &getFontLoader() const { return fontLoader; }

    const Size getScreenSize() const { return Size(resWidth, resHeight); }

    void addTouchMotionListener(const TouchMotionListener &listener) { touchMotionListeners.push_back(listener); }
    void addTouchTapEventListener(const TouchTapEventListener &listener) { touchTapEventListeners.push_back(listener); }
    void addMouseMotionListener(const MouseMotionListener &listener) { mouseMotionListeners.push_back(listener); }
    void addKeyInputListener(const KeyInputListener &listener) { keyInputListeners.push_back(listener); }
};

#endif // ENGINE_H
