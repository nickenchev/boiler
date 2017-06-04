#ifndef BLANKPART_H
#define BLANKPART_H

#include <boiler.h>
#include "core/part.h"
#include "core/logger.h"

class BlankPart : public Part
{
	Logger logger;
	
public:
    BlankPart();

	void onStart() override;
};

#endif /* BLANKPART_H */
