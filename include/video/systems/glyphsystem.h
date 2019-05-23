#ifndef GLYPHSYSTEM_H
#define GLYPHSYSTEM_H

#include "core/system.h"

namespace Boiler
{

class Renderer;

class GlyphSystem : public System
{
	const Renderer &renderer;
public:
	GlyphSystem(const Renderer &renderer) : System("Glyph System"), renderer(renderer) { }

	void update(ComponentStore &store, const double delta) override;
};

}

#endif /* GLYPHSYSTEM_H */
