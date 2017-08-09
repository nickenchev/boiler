#ifndef BLANKPART_H
#define BLANKPART_H

#include <memory>
#include <boiler.h>
#include "core/part.h"
#include "core/logger.h"

class SpriteSheet;

class BlankPart : public Part
{
	Logger logger;
    std::shared_ptr<const SpriteSheet> spriteSheet;
	
public:
    BlankPart();

	void onStart() override;
};

#endif /* BLANKPART_H */
