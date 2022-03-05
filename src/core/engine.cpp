#include <iostream>
#include <chrono>
#include <stdexcept>

#include "SDL_timer.h"
#include "SDL_video.h"
#include "core/common.h"
#include "video/opengl.h"
#include "boiler.h"
#include "video/renderer.h"
#include "core/part.h"
#include "core/entity.h"
#include "input/mousebuttonevent.h"
#include "input/keyinputevent.h"
#include "core/components/transformcomponent.h"
#include "core/components/rendercomponent.h"
#include "core/components/textcomponent.h"
#include "core/components/guicomponent.h"
#include "core/components/lightingcomponent.h"
#include "video/systems/guisystem.h"
#include "video/systems/lightingsystem.h"
#include "animation/components/animationcomponent.h"
#include "animation/systems/animationsystem.h"
#include "collision/collisioncomponent.h"
#include "collision/collisionsystem.h"

#define RENDERER_CLASS OpenGLRenderer

using namespace Boiler;

Engine::Engine(Renderer *renderer) : logger("Engine"), renderer(renderer),
									 baseDataPath(""), animator(ecs)
{
	guiSystem = nullptr;
	logger.log("Engine instance created");
	logger.log("Using renderer: " + this->renderer->getVersion());

    if (SDL_Init(SDL_INIT_TIMER) != 0)
	{
		throw std::runtime_error("Error initializing timer");
	}
	frameLag = 0;
	globalTime = 0;
}

Engine::~Engine()
{
	logger.log("Cleaning up");
	SDL_Quit();
}

void Engine::initialize(const Size &initialSize)
{
	initialize(nullptr, initialSize);
}

void Engine::initialize(std::unique_ptr<GUIHandler> guiHandler, const Size &initialSize)
{
	//baseDataPath = std::string(SDL_GetBasePath());

	// initialize basic engine stuff
	renderer->initialize(initialSize);

	System &animationSystem = ecs.getComponentSystems().registerSystem<AnimationSystem>(animator)
		.expects<AnimationComponent>()
		.expects<TransformComponent>();
	this->animationSystem = &animationSystem;

	System &collisionSystem = ecs.getComponentSystems().registerSystem<CollisionSystem>()
		.expects<CollisionComponent>()
		.expects<TransformComponent>();
	this->collisionSystem = &collisionSystem;

	System &lightingSys = ecs.getComponentSystems().registerSystem<LightingSystem>(*renderer)
		.expects<LightingComponent>();
	ecs.getComponentSystems().removeUpdate(lightingSys);
	this->lightingSystem = &lightingSys;

	System &renderSys = ecs.getComponentSystems().registerSystem<RenderSystem>(*renderer)
		.expects<TransformComponent>()
		.expects<RenderComponent>();
	ecs.getComponentSystems().removeUpdate(renderSys);
	this->renderSystem = &renderSys;

	/*
	System &glyphSys = ecs.getComponentSystems().registerSystem<GlyphSystem>(*renderer)
		.expects<TransformComponent>()
		.expects<TextComponent>();
	ecs.getComponentSystems().removeUpdate(glyphSys);
	this->glyphSystem = &glyphSys;
	*/

	if (guiHandler)
	{
		System &guiSys = ecs.getComponentSystems().registerSystem<GUISystem>(*renderer, std::move(guiHandler))
			.expects<GUIComponent>();
		ecs.getComponentSystems().removeUpdate(guiSys);
		this->guiSystem = &guiSys;
	}
}

void Engine::start(std::shared_ptr<Part> part)
{
	prevTime = SDL_GetTicks();

	//store the incoming part and start it
	this->part = part;
	this->part->onStart();

	logger.log("Starting main loop");
	
	//start processing events
	run();
}

void Engine::run()
{
	updateInterval = 1.0f / 90;
	running = true;

	while(running)
	{
		processEvents();
		step();
	}
	
	// wait for any renderer commands to finish before destructors kick in
	renderer->prepareShutdown();
	renderer->shutdown();
}

void Engine::step()
{
	//get the delta time
	Time64 currentTime = SDL_GetTicks64();
	Time frameDelta = (currentTime - prevTime) / 1000.0f;
	prevTime = currentTime;

	// frame update / catchup phase if lagging
	frameLag += frameDelta;
	while (frameLag >= updateInterval)
	{
		update(updateInterval);
		globalTime += updateInterval;
		frameLag -= updateInterval;
	}

	// render related systems only run during render phase
	// this is called before updateMatrices, wrong descriptor data
	// TODO: Handle GUI events differently
	if (renderer->beginRender())
	{
		lightingSystem->update(getEcs().getComponentStore(), frameDelta, globalTime);

		renderSystem->update(getEcs().getComponentStore(), frameDelta, globalTime);
		//glyphSystem->update(getEcs().getComponentStore(), frameDelta, globalTime);
		if (guiSystem) guiSystem->update(getEcs().getComponentStore(), frameDelta, globalTime);

		renderer->endRender();
	}
}

void Engine::processEvents()
{
	// poll input events
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_QUIT:
			{
				quit();
				break;
			}
			case SDL_WINDOWEVENT:
			{
				switch (event.window.event)
				{
					case SDL_WINDOWEVENT_RESIZED:
					{
						const Size newSize(static_cast<cgfloat>(event.window.data1), static_cast<cgfloat>(event.window.data2));
						renderer->resize(newSize);

						break;
					}
					case SDL_WINDOWEVENT_CLOSE:
					{
						quit();
						break;
					}
					case SDL_WINDOWEVENT_MINIMIZED:
					{
						logger.log("Pausing run loop");
						break;
					}
					case SDL_WINDOWEVENT_SHOWN:
					{
						logger.log("Resuming run loop");
						break;
					}
				}
				break;
			}
			case SDL_FINGERUP:
			{
				TouchTapEvent event(TapState::UP);
				for (const TouchTapEventListener &listener : touchTapEventListeners)
				{
					listener(event);
				}
				break;
			}
			case SDL_FINGERDOWN:
			{
				TouchTapEvent event(TapState::DOWN);
				for (const TouchTapEventListener &listener : touchTapEventListeners)
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
			case SDL_MOUSEMOTION:
			{
				MouseMotionEvent motionEvent(event.motion.x, event.motion.y,
									   event.motion.xrel, event.motion.yrel);
				for (const MouseMotionListener &listener : mouseMotionListeners)
				{
					listener(motionEvent);
				}
				break;
			}
			case SDL_KEYDOWN:
			{
				SDL_Keycode keyCode = event.key.keysym.sym;
				KeyInputEvent event(keyCode, ButtonState::DOWN);

				for (const KeyInputListener &listener : keyInputListeners)
				{
					listener(event);
				}
				break;
			}
			case SDL_KEYUP:
			{
				SDL_Keycode keyCode = event.key.keysym.sym;
				KeyInputEvent event(keyCode, ButtonState::UP);

				for (const KeyInputListener &listener : keyInputListeners)
				{
					listener(event);
				}
				break;
			}

			// GUI system needs to handle events independantly
			static_cast<GUISystem *>(guiSystem)->processEvent(event);
		}
	}
}

void Engine::update(const Time deltaTime)
{
	ecs.update(deltaTime, globalTime);
	part->update(updateInterval);
}
