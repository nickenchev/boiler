#ifndef FRAMEINFO_H
#define FRAMEINFO_H

#include "core/common.h"

namespace Boiler
{

struct FrameInfo
{
	short currentFrame;
	Time deltaTime, globalTime;
};

}

#endif /* FRAMEINFO_H */
