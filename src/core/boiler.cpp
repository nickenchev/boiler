#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
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

#define RENDERER_CLASS OpenGLRenderer

Boiler::Boiler() : spriteLoader(), imageLoader(), fontLoader(), baseDataPath(""), logger("Boiler")
{
	logger.log("Boiler instance created");
}

Boiler &Boiler::getInstance()
{
	static Boiler instance;
	return instance;
}

void Boiler::initialize(std::unique_ptr<Renderer> renderer, const int resWidth, const int resHeight)
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

	System &renderSys = ecs.getComponentSystems().registerSystem<RenderSystem>(*renderer)
		.expects<PositionComponent>()
		.expects<SpriteComponent>();
	ecs.getComponentSystems().removeUpdate(&renderSys);
	ecs.getComponentSystems().removeUpdate(&renderSys);
	ecs.getComponentSystems().removeUpdate(&renderSys);
	this->renderSystem = &renderSys;

	System &glyphSys = ecs.getComponentSystems().registerSystem<GlyphSystem>()
		.expects<PositionComponent>()
		.expects<TextComponent>();
	ecs.getComponentSystems().removeUpdate(&glyphSys);
	this->glyphSystem = &glyphSys;
}

void Boiler::start(std::shared_ptr<Part> part)
{
	//store the incoming part and start it
	this->part = part;
	this->part->onStart();

	logger.log("Starting main loop");
	
	//start processing events
	run();
}

void Boiler::run()
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

		processInput();

		while (frameLag >= frameInterval)
		{
			// TODO: Render system needs to not kick in when frame is lagging, only physics etc systems should be updated during frame lag catchup.
			// Need a clean way to essentially move the render() to outside the while loop here (after).
			update(frameInterval);
			frameLag -= frameInterval;
		} 
		
		renderer->beginRender();
		renderSystem->update(getEcs().getComponentStore(), frameDelta);
		glyphSystem->update(getEcs().getComponentStore(), frameDelta);
		renderer->endRender();
	}
}

void Boiler::processInput()
{
	// poll input events
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch(event.type)
		{
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
		}
	}
}

void Boiler::update(const double delta)
{
	ecs.update(delta);
}

Boiler::~Boiler()
{
	logger.log("Exiting");
	if (renderer)
	{
		renderer->shutdown();
	}
	SDL_Quit();
}
