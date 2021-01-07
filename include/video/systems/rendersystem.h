#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "core/common.h"
#include "core/system.h"
#include "core/lightsourceid.h"
#include "video/material.h"
#include "video/lightsource.h"

namespace Boiler
{

class Renderer;

class RenderSystem : public System
{
	Renderer &renderer;

public:
	RenderSystem(Renderer &renderer);

	void update(ComponentStore &store, const Time deltaTime, const Time globalTime) override;
};

}

#endif /* RENDERSYSTEM_H */
