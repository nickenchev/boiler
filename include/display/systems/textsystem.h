#ifndef TEXTSYSTEM_H
#define TEXTSYSTEM_H

#include "core/system.h"

namespace Boiler
{

class TextSystem : public System
{
public:
    TextSystem();
    virtual ~TextSystem() { }

	void update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, ComponentStore &store) override;
};

}

#endif /* TEXTSYSTEM_H */
