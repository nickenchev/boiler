#include <iostream>
#include <chrono>
#include <stdexcept>

#include "core/common.h"
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
#include "physics/physicssystem.h"

using namespace Boiler;

Engine::Engine(Renderer *renderer) : logger("Engine"), renderer(renderer),
									 baseDataPath(""), animator(ecs)
{
	animationSystem = nullptr;
	cameraSystem = nullptr;
	collisionSystem = nullptr;
	debugRenderSystem = nullptr;
	guiSystem = nullptr;
	inputSystem = nullptr;
	lightingSystem = nullptr;
	movementSystem = nullptr;
	physicsSystem = nullptr;
	renderSystem = nullptr;
	textSystem = nullptr;
	logger.log("Engine instance created");
	logger.log("Using renderer: {}", this->renderer->getVersion());

	updateInterval = 1 / 60.0f;
	frameLag = 0;
	globalTime = 0;
	currentFrame = 0;
	frameCount = 0;
	mouseRelativeMode = true;
	cleanedUp = false;
	running = false;
}

void Engine::initialize(const Size &initialSize)
{
	cleanedUp = false;
	running = true;
	renderer->initialize(initialSize);

	updateInterval = (1.0f / 60);
	prevTime = std::chrono::high_resolution_clock::now();

    // initialize basic engine stuff
    System &inputSystem = ecs.getComponentSystems().registerSystem<InputSystem>(SystemStage::IO, *this);
    this->inputSystem = &inputSystem;

    System &animationSystem = ecs.getComponentSystems().registerSystem<AnimationSystem>(SystemStage::SIMULATION, animator);
    this->animationSystem = &animationSystem;

    System &movementSystem = ecs.getComponentSystems().registerSystem<MovementSystem>(SystemStage::SIMULATION);
    this->movementSystem = &movementSystem;

    System &physicsSystem = ecs.getComponentSystems().registerSystem<PhysicsSystem>(SystemStage::SIMULATION, matrixCache);
    this->physicsSystem = &physicsSystem;

    System &lightingSys = ecs.getComponentSystems().registerSystem<LightingSystem>(SystemStage::RENDER);
    this->lightingSystem = &lightingSys;

    System &renderSys = ecs.getComponentSystems().registerSystem<RenderSystem>(SystemStage::RENDER, matrixCache);
    this->renderSystem = &renderSys;

    System &textSys = ecs.getComponentSystems().registerSystem<TextSystem>(SystemStage::RENDER);
    this->textSystem = &textSys;

    //System &debugRenderSys = ecs.getComponentSystems().registerSystem<DebugRenderSystem>(SystemStage::RENDER, *renderer, matrixCache);
    //this->debugRenderSystem = &debugRenderSys;

    System &guiSys = ecs.getComponentSystems().registerSystem<GUISystem>(SystemStage::RENDER, *renderer);
    this->guiSystem = &guiSys;

    System &cameraSystem = ecs.getComponentSystems().registerSystem<CameraSystem>(SystemStage::RENDER);
    this->cameraSystem = &cameraSystem;
}

Engine::~Engine()
{
	logger.log("Cleaning up");
}

void Engine::step(FrameInfo &frameInfo)
{
	auto newTime = std::chrono::high_resolution_clock::now();
	frameInfo.currentFrame = currentFrame;
	frameInfo.deltaTime = updateInterval;
	frameInfo.frameTime = std::chrono::duration<Time, std::chrono::seconds::period>(newTime - prevTime).count();
	frameInfo.globalTime = globalTime;
	frameInfo.frameCount = frameCount;

	// input and IO systems
    processEvents(frameInfo);
	ecs.update(*renderer, renderer->getAssetSet(), frameInfo, SystemStage::IO);

	currentFrame = (currentFrame + 1) % renderer->getMaxFramesInFlight();

	// frame update / catchup phase if lagging
	frameLag += frameInfo.frameTime;
	frameInfo.globalTime = globalTime;
	while (frameLag >= updateInterval)
	{
        part->update(frameInfo);
		ecs.update(*renderer, renderer->getAssetSet(), frameInfo, SystemStage::SIMULATION);
		ecs.update(*renderer, renderer->getAssetSet(), frameInfo, SystemStage::USER_SIMULATION);
		ecs.endFrame();

		globalTime += updateInterval;
		frameLag -= updateInterval;
	}

	// render related systems only run during render phase
	// this is called before updateMatrices, wrong descriptor data
	if (renderer->prepareFrame(frameInfo))
	{
		ecs.update(*renderer, renderer->getAssetSet(), frameInfo, SystemStage::RENDER);
		renderer->displayFrame(frameInfo, renderer->getAssetSet());
	}
	matrixCache.reset();
	frameCount++;
	prevTime = newTime;
}

void Engine::processEvents(FrameInfo &frameInfo)
{
}

void Engine::setMouseRelativeMode(bool enabled)
{
	mouseRelativeMode = enabled;
	//SDL_SetRelativeMouseMode(mouseRelativeMode ? SDL_TRUE : SDL_FALSE);
	//static_cast<DebugRenderSystem *>(debugRenderSystem)->setEnabled(!mouseRelativeMode);
}

void Engine::setPart(std::shared_ptr<Part> part)
{
    this->part = part;
    part->onStart();
}

void Engine::shutdown()
{
	if (!cleanedUp)
	{
		// wait for any renderer commands to finish before destructors kick in
		renderer->prepareShutdown();
		renderer->shutdown();

		cleanedUp = true;
	}
}
