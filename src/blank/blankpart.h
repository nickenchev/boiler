#ifndef BLANKPART_H
#define BLANKPART_H

#include <boiler.h>
#include "core/logger.h"

class BlankPart : public Entity
{
	Logger logger;
	
public:
    BlankPart();

    void onCreate() override;
    void update() override;
};

#endif /* BLANKPART_H */
