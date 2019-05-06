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
#include "video/systems/rendersystem.h"
#include "video/systems/glyphsystem.h"
#include "video/systems/guisystem.h"
#include "video/guihandler.h"
#include "logger.h"

namespace Boiler
{

class Entity;
class Renderer;
class Part;

typedef std::function<void(const TouchMotionEvent &event)> TouchMotionListener;
typedef std::function<void(const TouchTapEvent &event)> TouchTapEventListener;
typedef std::function<void(const MouseMotionEvent &event)> MouseMotionListener;
typedef std::function<void(const KeyInputEvent &event)> KeyInputListener;

class Engine
{
	Logger logger;
	EntityComponentSystem ecs;
    std::string baseDataPath;
    std::unique_ptr<Renderer> renderer;
	System *renderSystem, *glyphSystem, *guiSystem;

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
    void processEvents();
    void update(const double delta);

    std::shared_ptr<Part> part;

    // singleton
    Engine();
    ~Engine();

public:
    Engine(const Engine &) = delete;
    void operator=(const Engine &s) = delete;

    static Engine &getInstance();

    void initialize(std::unique_ptr<Renderer> renderer, std::unique_ptr<GUIHandler> guiHandler, const int resWidth, const int resHeight);
    void start(std::shared_ptr<Part> part);
    void quit() { running = false; }

	EntityComponentSystem &getEcs() { return ecs; }
    Renderer &getRenderer() const { return *renderer.get(); }
    std::shared_ptr<Part> getPart() const { return part; }

    const SpriteLoader &getSpriteLoader() const { return spriteLoader; }
    const ImageLoader &getImageLoader() const { return imageLoader; }
    const FontLoader &getFontLoader() const { return fontLoader; }

    void addTouchMotionListener(const TouchMotionListener &listener) { touchMotionListeners.push_back(listener); }
    void addTouchTapEventListener(const TouchTapEventListener &listener) { touchTapEventListeners.push_back(listener); }
    void addMouseMotionListener(const MouseMotionListener &listener) { mouseMotionListeners.push_back(listener); }
    void addKeyInputListener(const KeyInputListener &listener) { keyInputListeners.push_back(listener); }
};

}

#endif // ENGINE_H
