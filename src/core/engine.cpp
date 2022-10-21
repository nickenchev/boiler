#include <iostream>
#include <chrono>
#include <stdexcept>

#include <SDL.h>
#include "core/common.h"
#include "display/opengl.h"
#include "boiler.h"
#include "display/renderer.h"
#include "core/part.h"
#include "core/entity.h"
#include "input/mousebuttonevent.h"
#include "input/keyinputevent.h"
#include "input/inputsystem.h"
#include "core/frameinfo.h"
#include "core/components/transformcomponent.h"
#include "core/components/rendercomponent.h"
#include "core/components/textcomponent.h"
#include "core/components/guicomponent.h"
#include "core/components/lightingcomponent.h"
#include "display/systems/guisystem.h"
#include "display/systems/lightingsystem.h"
#include "display/systems/textsystem.h"
#include "display/systems/debugrendersystem.h"
#include "animation/components/animationcomponent.h"
#include "animation/systems/animationsystem.h"
#include "camera/camerasystem.h"
#include "physics/movementsystem.h"
#include "physics/collisionsystem.h"
#include "physics/collisioncomponent.h"
#include "physics/transformsystem.h"

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
	currentFrame = 0;
	frameCount = 0;
	mouseRelativeMode = true;
	SDL_SetRelativeMouseMode(SDL_TRUE);
	running = false;
}

Engine::~Engine()
{
	logger.log("Cleaning up");
	SDL_Quit();
}

void Engine::initialize(const Size &initialSize)
{
	// TODO: Figure out base path stuff
	//baseDataPath = std::string(SDL_GetBasePath());

	// initialize basic engine stuff
	renderer->initialize(initialSize);

	System &inputSystem = ecs.getComponentSystems().registerSystem<InputSystem>(*this);
	this->inputSystem = &inputSystem;

	System &cameraSystem = ecs.getComponentSystems().registerSystem<CameraSystem>();
	this->cameraSystem = &cameraSystem;

	System &movementSystem = ecs.getComponentSystems().registerSystem<MovementSystem>();
	this->movementSystem = &movementSystem;

	System &animationSystem = ecs.getComponentSystems().registerSystem<AnimationSystem>(animator);
	this->animationSystem = &animationSystem;

	System &collisionSystem = ecs.getComponentSystems().registerSystem<CollisionSystem>();
	this->collisionSystem = &collisionSystem;

	System &lightingSys = ecs.getComponentSystems().registerSystem<LightingSystem>();
	this->lightingSystem = &lightingSys;
	ecs.getComponentSystems().removeUpdate(lightingSys);

	System &transformSys = ecs.getComponentSystems().registerSystem<TransformSystem>();

	System &renderSys = ecs.getComponentSystems().registerSystem<RenderSystem>();
	ecs.getComponentSystems().removeUpdate(renderSys);
	this->renderSystem = &renderSys;

	System &textSys = ecs.getComponentSystems().registerSystem<TextSystem>();
	ecs.getComponentSystems().removeUpdate(textSys);
	this->textSystem = &textSys;
	/*
	System &glyphSys = ecs.getComponentSystems().registerSystem<GlyphSystem>(*renderer)
		.expects<TransformComponent>()
		.expects<TextComponent>();
	ecs.getComponentSystems().removeUpdate(glyphSys);
	this->glyphSystem = &glyphSys;
	*/

	System &guiSys = ecs.getComponentSystems().registerSystem<GUISystem>(*renderer)
		.expects<GUIComponent>();
	ecs.getComponentSystems().removeUpdate(guiSys);
	this->guiSystem = &guiSys;

	System &debugRenderSys = ecs.getComponentSystems().registerSystem<DebugRenderSystem>(*renderer);
	ecs.getComponentSystems().removeUpdate(debugRenderSys);
	this->debugRenderSystem = &debugRenderSys;
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
		currentFrame = (currentFrame + 1) % renderer->getMaxFramesInFlight();
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
	frameInfo.frameCount = frameCount;

	update(renderer->getAssetSet(), frameInfo);
	globalTime += deltaTime;

	// render related systems only run during render phase
	// this is called before updateMatrices, wrong descriptor data
	if (renderer->prepareFrame(frameInfo))
	{
		lightingSystem->update(*renderer, renderer->getAssetSet(), frameInfo, ecs);
		renderSystem->update(*renderer, renderer->getAssetSet(), frameInfo, ecs);
		textSystem->update(*renderer, renderer->getAssetSet(), frameInfo, ecs);
		if (guiSystem)
		{
			guiSystem->update(*renderer, renderer->getAssetSet(), frameInfo, ecs);
		}
		debugRenderSystem->update(*renderer, renderer->getAssetSet(), frameInfo, ecs);

		renderer->displayFrame(frameInfo, renderer->getAssetSet());
	}
	frameCount++;
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
						break;
					}
					case SDL_WINDOWEVENT_SHOWN:
					{
						break;
					}
				}
				break;
			}
			case SDL_FINGERUP:
			{
				/*
				TouchTapEvent event(TapState::UP);
				for (const TouchTapEventListener &listener : touchTapEventListeners)
				{
					listener(event);
				}
				*/
				break;
			}
			case SDL_FINGERDOWN:
			{
				/*
				TouchTapEvent event(TapState::DOWN);
				for (const TouchTapEventListener &listener : touchTapEventListeners)
				{
					listener(event);
				}
				*/
				break;
			}
			case SDL_FINGERMOTION:
			{
				/*
				TouchMotionEvent motionEvent(event.tfinger.x, event.tfinger.y,
											 event.tfinger.dx, event.tfinger.dy);
				for (const TouchMotionListener &listener : touchMotionListeners)
				{
					listener(motionEvent);
				}
				*/
				break;
			}
			case SDL_MOUSEWHEEL:
			{
				static_cast<GUISystem *>(guiSystem)->mouseWheel(event.wheel.preciseX, event.wheel.preciseY);
				break;
			}
			case SDL_MOUSEMOTION:
			{
				frameInfo.mouseXDistance += event.motion.xrel;
				frameInfo.mouseYDistance += event.motion.yrel;
				if (!mouseRelativeMode)
				{
					static_cast<GUISystem *>(guiSystem)->mouseMove(event.motion.x, event.motion.y);
				}
				break;
			}
			case SDL_MOUSEBUTTONDOWN:
			{
				if (!mouseRelativeMode)
				{
					static_cast<GUISystem *>(guiSystem)->mouseButton(event.button.button, true);
				}
				break;
			}
			case SDL_MOUSEBUTTONUP:
			{
				if (!mouseRelativeMode)
				{
					static_cast<GUISystem *>(guiSystem)->mouseButton(event.button.button, false);
				}
				break;
			}
			case SDL_KEYDOWN:
			{
				static_cast<GUISystem *>(guiSystem)->keyEvent(event.key.keysym.sym, true);
				static_cast<GUISystem *>(guiSystem)->keyMods(event.key.keysym.mod);

				SDL_Keycode keyCode = event.key.keysym.sym;
				KeyInputEvent event(keyCode, ButtonState::DOWN);
				frameInfo.keyInputEvents.addEvent(event);

				break;
			}
			case SDL_KEYUP:
			{
				if (event.key.keysym.sym == SDLK_BACKQUOTE)
				{
					mouseRelativeMode = !mouseRelativeMode;
					SDL_SetRelativeMouseMode(mouseRelativeMode ? SDL_TRUE : SDL_FALSE);
				}
				static_cast<GUISystem *>(guiSystem)->keyEvent(event.key.keysym.sym, false);

				SDL_Keycode keyCode = event.key.keysym.sym;
				KeyInputEvent event(keyCode, ButtonState::UP);
				frameInfo.keyInputEvents.addEvent(event);
				break;
			}
			case SDL_TEXTINPUT:
			{
				static_cast<GUISystem *>(guiSystem)->textInput(event.text.text);
				break;
			}
		}
	}
}

void Engine::update(AssetSet &assetSet, const FrameInfo &frameInfo)
{
	ecs.update(*renderer, assetSet, frameInfo);
	part->update(frameInfo);
}
