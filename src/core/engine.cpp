#include <iostream>
#include "video/opengl.h"
#include "boiler.h"
#include "video/renderer.h"
#include "core/part.h"
#include "core/entity.h"
#include "input/mousebuttonevent.h"
#include "input/keyinputevent.h"
#include "core/components/positioncomponent.h"
#include "core/components/spritecomponent.h"
#include "core/components/textcomponent.h"
#include "core/components/guicomponent.h"
#include "video/systems/guisystem.h"

#define RENDERER_CLASS OpenGLRenderer

using namespace Boiler;

Engine::Engine() : spriteLoader(), imageLoader(), fontLoader(), baseDataPath(""), logger("Engine")
{
	logger.log("Engine instance created");
}

Engine &Engine::getInstance()
{
	static Engine instance;
	return instance;
}

void Engine::initialize(std::unique_ptr<Renderer> renderer, std::unique_ptr<GUIHandler> guiHandler, const int resWidth, const int resHeight)
{
	if (this->renderer != nullptr)
	{
		this->renderer->shutdown();
	}

	logger.log("Initializing...");
	assert(renderer != nullptr); // No renderer provided

	// initialization was successful
	logger.log("Using renderer: " + renderer->getVersion());
	//baseDataPath = std::string(SDL_GetBasePath());

	// initialize basic engine stuff
	frameInterval = 1.0f / 60.0f; // 60fps
	this->renderer = std::move(renderer);
	getRenderer().initialize(Size(resWidth, resHeight));

	System &renderSys = ecs.getComponentSystems().registerSystem<RenderSystem>(*this->renderer)
		.expects<PositionComponent>()
		.expects<SpriteComponent>();
	ecs.getComponentSystems().removeUpdate(&renderSys);
	this->renderSystem = &renderSys;

	System &glyphSys = ecs.getComponentSystems().registerSystem<GlyphSystem>(*this->renderer)
		.expects<PositionComponent>()
		.expects<TextComponent>();
	ecs.getComponentSystems().removeUpdate(&glyphSys);
	this->glyphSystem = &glyphSys;

	System &guiSys = ecs.getComponentSystems().registerSystem<GUISystem>(*this->renderer, std::move(guiHandler))
		.expects<GUIComponent>();
	ecs.getComponentSystems().removeUpdate(&guiSys);
	this->guiSystem = &guiSys;
}

void Engine::start(std::shared_ptr<Part> part)
{
	//store the incoming part and start it
	this->part = part;
	this->part->onStart();

	logger.log("Starting main loop");
	
	//start processing events
	run();
}

void Engine::run()
{
	double prevTime = SDL_GetTicks();
	double frameLag = 0.0f;

	running = true;
	while(running)
	{
		//get the delta time
		double currentTime = SDL_GetTicks();
		double frameDelta = (currentTime - prevTime) / 1000.0f;
		prevTime = currentTime;
		frameLag += frameDelta;

		processEvents();

		// frame update / catchup phase if lagging
		while (frameLag >= frameInterval)
		{
			update(frameInterval);
			part->update(frameInterval);
			frameLag -= frameInterval;
		} 
		
		// render related systems only run during render phase
		// TODO: Handle GUI events differently
		renderer->beginRender();
		renderSystem->update(getEcs().getComponentStore(), frameDelta);
		glyphSystem->update(getEcs().getComponentStore(), frameDelta);
		guiSystem->update(getEcs().getComponentStore(), frameDelta);
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
					case SDL_WINDOWEVENT_CLOSE:
					{
						quit();
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

void Engine::update(const double delta)
{
	ecs.update(delta);
}

Engine::~Engine()
{
	logger.log("Exiting");
	if (renderer)
	{
		renderer->shutdown();
	}
	SDL_Quit();
}
