#ifndef GLYPHSYSTEM_H
#define GLYPHSYSTEM_H

#include "core/system.h"

class GlyphSystem : public System
{
public:
	GlyphSystem() : System("Glyph System") { }

	void update(ComponentStore &store, const double delta) const override;
};

#endif /* GLYPHSYSTEM_H */
