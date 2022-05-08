#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "core/common.h"
#include "core/frameinfo.h"
#include "core/system.h"
#include "core/lightsourceid.h"
#include "display/material.h"
#include "display/lightsource.h"

namespace Boiler
{

class Renderer;

class RenderSystem : public System
{
	Renderer &renderer;

public:
	RenderSystem(Renderer &renderer);

	void update(const FrameInfo &frameInfo, ComponentStore &store) override;
};

}

#endif /* RENDERSYSTEM_H */
