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

Engine::Engine() : logger("Engine"),baseDataPath(""), animator(ecs)
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

	updateInterval = 1 / 60.0f;
	frameLag = 0;
	globalTime = 0;
	currentFrame = 0;
	frameCount = 0;
	mouseRelativeMode = true;
	cleanedUp = false;
	running = false;
}

void Engine::initialize(Renderer *renderer)
{
	this->renderer = renderer;
	logger.log("Using renderer: {}", this->renderer->getVersion());

	cleanedUp = false;
	running = true;
	updateInterval = (1.0f / 60);

    // initialize basic engine stuff
    System &inputSystem = ecs.getComponentSystems().registerSystem<InputSystem>(SystemStage::IO, *this);
    this->inputSystem = &inputSystem;

    System &animationSystem = ecs.getComponentSystems().registerSystem<AnimationSystem>(SystemStage::SIMULATION, animator);
    this->animationSystem = &animationSystem;

    System &movementSystem = ecs.getComponentSystems().registerSystem<MovementSystem>(SystemStage::SIMULATION);
    this->movementSystem = &movementSystem;

    System &physicsSystem = ecs.getComponentSystems().registerSystem<PhysicsSystem>(SystemStage::SIMULATION, matrixCache);
    this->physicsSystem = &physicsSystem;

    System &debugRenderSys = ecs.getComponentSystems().registerSystem<DebugRenderSystem>(SystemStage::RENDER, *renderer, matrixCache);
    this->debugRenderSystem = &debugRenderSys;

    System &guiSys = ecs.getComponentSystems().registerSystem<GUISystem>(SystemStage::RENDER, *renderer);
    this->guiSystem = &guiSys;

    System &cameraSystem = ecs.getComponentSystems().registerSystem<CameraSystem>(SystemStage::RENDER, *this);
    this->cameraSystem = &cameraSystem;

    System &lightingSys = ecs.getComponentSystems().registerSystem<LightingSystem>(SystemStage::RENDER);
    this->lightingSystem = &lightingSys;

    System &textSys = ecs.getComponentSystems().registerSystem<TextSystem>(SystemStage::RENDER);
    this->textSystem = &textSys;

    System &renderSys = ecs.getComponentSystems().registerSystem<RenderSystem>(SystemStage::RENDER, matrixCache);
    this->renderSystem = &renderSys;

	prevTime = std::chrono::high_resolution_clock::now();
}

Engine::~Engine()
{
	assert(cleanedUp);
}

void Boiler::Engine::beginFrame(FrameInfo &frameInfo)
{
	frameInfo.currentTime = std::chrono::high_resolution_clock::now();
	frameInfo.currentFrame = currentFrame;
	frameInfo.deltaTime = updateInterval;
	frameInfo.frameTime = std::chrono::duration<Time, std::chrono::seconds::period>(frameInfo.currentTime - prevTime).count();
	frameInfo.frameCount = frameCount;

	// input and IO systems
    processEvents(frameInfo);
}

void Engine::step(FrameInfo &frameInfo)
{
	// get the appropriate assetset
	AssetSet &assetSet = getPart()->getAssetSet();

	ecs.update(*renderer, assetSet, frameInfo, SystemStage::IO);

	currentFrame = (currentFrame + 1) % renderer->getMaxFramesInFlight();

	// frame update / catchup phase if lagging
	frameLag += frameInfo.frameTime;
	while (frameLag >= updateInterval)
	{
        part->update(frameInfo);
		ecs.update(*renderer, assetSet, frameInfo, SystemStage::SIMULATION);
		ecs.update(*renderer, assetSet, frameInfo, SystemStage::USER_SIMULATION);

		frameLag -= updateInterval;
		globalTime += updateInterval;
		frameInfo.globalTime = globalTime;
		ecs.endFrame();
	}
}

void Boiler::Engine::performRender(FrameInfo &frameInfo)
{
	AssetSet &assetSet = getPart()->getAssetSet();
	// render related systems only run during render phase
	// this is called before updateMatrices, wrong descriptor data
	if (renderer->prepareFrame(frameInfo))
	{
		ecs.update(*renderer, assetSet, frameInfo, SystemStage::RENDER);
		renderer->finalizeFrame(frameInfo, assetSet);
	}
}

void Boiler::Engine::endFrame(FrameInfo &frameInfo)
{
	matrixCache.reset();
	frameCount++;
	prevTime = frameInfo.currentTime;
}

void Engine::processEvents(FrameInfo &frameInfo)
{
	platformHandler(static_cast<InputSystem &>(*inputSystem));
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
		renderer->releaseAssetSet(part->getAssetSet());
		renderer->shutdown();

		logger.log("Shutdown complete");
		cleanedUp = true;
	}
}
