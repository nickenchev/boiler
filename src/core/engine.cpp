#include <iostream>
#include <chrono>
#include <stdexcept>

#include <SDL2/SDL.h>
#include "core/common.h"
#include "display/opengl.h"
#include "boiler.h"
#include "display/renderer.h"
#include "core/part.h"
#include "core/entity.h"
#include "input/mousebuttonevent.h"
#include "input/keyinputevent.h"
#include "core/frameinfo.h"
#include "core/components/transformcomponent.h"
#include "core/components/rendercomponent.h"
#include "core/components/textcomponent.h"
#include "core/components/guicomponent.h"
#include "core/components/lightingcomponent.h"
#include "display/systems/guisystem.h"
#include "display/systems/lightingsystem.h"
#include "animation/components/animationcomponent.h"
#include "animation/systems/animationsystem.h"
#include "camera/camerasystem.h"

using namespace Boiler;
constexpr unsigned int maxFramesInFlight = 3;

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
	currentFrame = 0;
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

	System &cameraSystem = ecs.getComponentSystems().registerSystem<CameraSystem>();
	this->cameraSystem = &cameraSystem;

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
		logger.log("Setting up GUI handler");
		System &guiSys = ecs.getComponentSystems().registerSystem<GUISystem>(std::move(guiHandler))
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

	logger.log("Running main loop");
	
	//start processing events
	run();
}

void Engine::run()
{
	updateInterval = (1.0f / 90);
	running = true;

	while(running)
	{
		FrameInfo frameInfo;
		processEvents(frameInfo);
		step(frameInfo);
		currentFrame = (currentFrame + 1) % maxFramesInFlight;
	}
	
	// wait for any renderer commands to finish before destructors kick in
	renderer->prepareShutdown();
	renderer->shutdown();
}

void Engine::step(FrameInfo &frameInfo)
{
	//get the delta time
	Time64 currentTime = SDL_GetTicks64();
	Time deltaTime = (currentTime - prevTime) / 1000.0f;
	prevTime = currentTime;

	// frame update / catchup phase if lagging
	/*
	FrameInfo frameInfo(currentFrame, updateInterval, globalTime);
	frameLag += frameDelta;
	while (frameLag >= updateInterval)
	{
		update(frameInfo);
		globalTime += updateInterval;
		frameLag -= updateInterval;
	}
	*/
	frameInfo.currentFrame = currentFrame;
	frameInfo.deltaTime = deltaTime;
	frameInfo.globalTime = globalTime;

	update(frameInfo);
	globalTime += deltaTime;

	// render related systems only run during render phase
	// this is called before updateMatrices, wrong descriptor data
	if (renderer->prepareFrame(frameInfo))
	{
		lightingSystem->update(frameInfo, getEcs().getComponentStore());

		renderSystem->update(frameInfo, getEcs().getComponentStore());
		//glyphSystem->update(frameInfo, getEcs().getComponentStore());
		if (guiSystem)
		{
			guiSystem->update(frameInfo, getEcs().getComponentStore());
		}
		renderer->displayFrame(frameInfo);
	}
}

void Engine::processEvents(FrameInfo &frameInfo)
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

void Engine::update(const FrameInfo &frameInfo)
{
	ecs.update(frameInfo);
	part->update(frameInfo.deltaTime);
}
