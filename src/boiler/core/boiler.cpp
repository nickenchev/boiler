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

#define RENDERER_CLASS OpenGLRenderer

Boiler::Boiler() : spriteLoader(), imageLoader(), fontLoader(), baseDataPath("")
{
}

Boiler &Boiler::getInstance()
{
	static Boiler instance;
	return instance;
}

void Boiler::initialize(std::unique_ptr<Renderer> renderer, const int resWidth, const int resHeight)
{
	std::cout << "* Initializing..." << std::endl;
	assert(renderer != nullptr); // No renderer provided

	// initialization was successful
	std::cout << " - Using Renderer: " << renderer->getVersion() << std::endl;
	//baseDataPath = std::string(SDL_GetBasePath());

	// initialize basic engine stuff
	frameInterval = 1.0f / 60.0f; // 60fps
	this->renderer = std::move(renderer);
	getRenderer().initialize(Size(resWidth, resHeight));
}

void Boiler::start(std::shared_ptr<Part> part)
{
	//store the incoming part and start it
	this->part = part;
	this->part->onStart();

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
			update(frameInterval);
			frameLag -= frameInterval;
		} 
		// pass normalized 0.0 - 1.0 so renderer knows how far between frames we are
		const double normalized = frameLag / frameInterval;
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
	SDL_Quit();
}
