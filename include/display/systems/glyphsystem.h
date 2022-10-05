#ifndef GLYPHSYSTEM_H
#define GLYPHSYSTEM_H

#include "core/common.h"
#include "core/system.h"

namespace Boiler
{

class Renderer;

class GlyphSystem : public System
{
public:
	GlyphSystem() : System("Glyph System") { }

    void update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, EntityComponentSystem &ecs) override;
};

}

#endif /* GLYPHSYSTEM_H */
