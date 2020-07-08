#ifndef ENGINE_H
#define ENGINE_H

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include "spriteloader.h"
#include "video/imaging/imageloader.h"
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
	std::unique_ptr<Renderer> renderer;
	EntityComponentSystem ecs;
    std::string baseDataPath;
	System *renderSystem, *glyphSystem, *guiSystem, *lightingSystem;

    std::vector<TouchMotionListener> touchMotionListeners;
    std::vector<TouchTapEventListener> touchTapEventListeners;
    std::vector<MouseMotionListener> mouseMotionListeners;
    std::vector<KeyInputListener> keyInputListeners;

    bool running = true;
	bool paused = false;

    double frameInterval;

    void run();
    void processEvents();
    void update(const double delta);

    std::shared_ptr<Part> part;

public:
	Engine(std::unique_ptr<Renderer> &&renderer);
	~Engine();
    Engine(const Engine &) = delete;
    void operator=(const Engine &s) = delete;

    void initialize(const int resWidth, const int resHeight);
    void initialize(std::unique_ptr<GUIHandler> guiHandler, const int resWidth, const int resHeight);
	void shutdown();
    void start(std::shared_ptr<Part> part);
    void quit() { running = false; }

	Renderer &getRenderer() { return *renderer; }
	EntityComponentSystem &getEcs() { return ecs; }
    std::shared_ptr<Part> getPart() const { return part; }

    void addTouchMotionListener(const TouchMotionListener &listener) { touchMotionListeners.push_back(listener); }
    void addTouchTapEventListener(const TouchTapEventListener &listener) { touchTapEventListeners.push_back(listener); }
    void addMouseMotionListener(const MouseMotionListener &listener) { mouseMotionListeners.push_back(listener); }
    void addKeyInputListener(const KeyInputListener &listener) { keyInputListeners.push_back(listener); }
};

}

#endif // ENGINE_H
