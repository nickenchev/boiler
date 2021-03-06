#ifndef GLYPHSYSTEM_H
#define GLYPHSYSTEM_H

#include "core/common.h"
#include "core/system.h"

namespace Boiler
{

class Renderer;

class GlyphSystem : public System
{
	Renderer &renderer;
public:
	GlyphSystem(Renderer &renderer) : System("Glyph System"), renderer(renderer) { }

	void update(ComponentStore &store, const Time deltaTime, const Time globalTime) override;
};

}

#endif /* GLYPHSYSTEM_H */
