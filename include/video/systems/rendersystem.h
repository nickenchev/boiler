#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "core/system.h"

class RenderSystem : public System
{
public:
	void update(ComponentStore &store, const double delta) const override;
};


#endif /* RENDERSYSTEM_H */
