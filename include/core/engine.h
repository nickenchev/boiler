#ifndef ENGINE_H
#define ENGINE_H

#include <chrono>
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include "core/common.h"
#include "spriteloader.h"
#include "display/imaging/imageloader.h"
#include "display/fontloader.h"
#include "input/touchmotionevent.h"
#include "input/touchtapevent.h"
#include "input/mousemotionevent.h"
#include "input/keyinputevent.h"
#include "core/entitycomponentsystem.h"
#include "display/systems/rendersystem.h"
#include "display/systems/guisystem.h"
#include "logger.h"
#include "animation/common.h"
#include "animation/animator.h"
#include "core/matrixcache.h"

namespace Boiler
{

class Entity;
class Renderer;
class Part;

class Engine
{
	Time globalTime, frameLag;
	short currentFrame;
	unsigned long frameCount;

	Logger logger;
	Renderer *renderer;
	EntityComponentSystem ecs;
    std::string baseDataPath;
	MatrixCache matrixCache;
    System *renderSystem, *textSystem, *guiSystem, *debugRenderSystem,
		*lightingSystem, *animationSystem, *physicsSystem, *collisionSystem,
		*cameraSystem, *inputSystem, *movementSystem;
	Animator animator;

    bool cleanedUp, running;
	bool mouseRelativeMode;
    Time updateInterval;
	std::chrono::steady_clock::time_point prevTime;

    void processEvents(FrameInfo &frameInfo);

    std::shared_ptr<Part> part;

public:
	Engine(Renderer *renderer);
	~Engine();
    Engine(const Engine &) = delete;
    void operator=(const Engine &s) = delete;

	void initialize(const Size &initialSize);
	void shutdown();
    void start(std::shared_ptr<Part> part);
	void step();
    void quit() { running = false; }

	RenderSystem &getRenderSystem()
	{
		return static_cast<RenderSystem &>(*renderSystem);
	}

	Renderer &getRenderer() { return *renderer; }
	EntityComponentSystem &getEcs() { return ecs; }
	Animator &getAnimator() { return animator; }

    std::shared_ptr<Part> getPart() const { return part; }
    void setPart(std::shared_ptr<Part> part);

	bool isRunning() const { return running; }
};

}

#endif // ENGINE_H
