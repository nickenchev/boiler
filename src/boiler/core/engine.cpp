#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include "video/opengl.h"
#include "engine.h"
#include "video/renderer.h"
#include "part.h"
#include "entity.h"
#include "input/mousebuttonevent.h"
#include "input/keyinputevent.h"

#define RENDERER_CLASS OpenGLRenderer

Engine::Engine() : spriteLoader(), imageLoader(), fontLoader(), baseDataPath("")
{
}

Engine &Engine::getInstance()
{
	static Engine instance;
	return instance;
}

void Engine::initialize(std::unique_ptr<Renderer> renderer, const int resWidth, const int resHeight)
{
	std::cout << "* Initializing..." << std::endl;
	assert(renderer != nullptr); // No renderer provided

	this->resWidth = resWidth;
	this->resHeight = resHeight;

	// initialization was successful
	std::cout << " - Using Renderer: " << renderer->getVersion() << std::endl;
	//baseDataPath = std::string(SDL_GetBasePath());

	// initialize basic engine stuff
	frameInterval = 1.0f / 60.0f; // 60fps
	this->renderer = std::move(renderer);
	getRenderer().initialize(getScreenSize());
}

void Engine::start(std::shared_ptr<Entity> part)
{
	//store the incoming part and start it
	this->part = part;
	part->onCreate();

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

		processInput();

		while (frameLag >= frameInterval)
		{
			update(frameInterval);
			frameLag -= frameInterval;
		} 
		// pass normalized 0.0 - 1.0 so renderer knows how far between frames we are
		render(frameLag / frameInterval);
	}
}

void Engine::processInput()
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

void Engine::updateEntities(const std::vector<std::shared_ptr<Entity>> &entities)
{
	for (auto &entity : entities)
	{
		entity->update();

		// update children the child entities
		if (entity->getChildren().size() > 0)
		{
			updateEntities(entity->getChildren());
		}
	}
}

void Engine::update(const float delta)
{
	part->update();

	const auto &entities = part->getChildren();
	updateEntities(entities);
}

void Engine::render(const float delta)
{
	renderer->render();
}

Engine::~Engine()
{
	SDL_Quit();
}
