#include "core/logger.h"
#include "core/logdestination.h"
#include "core/stdoutdestination.h"

using namespace Boiler;

Logger::Logger(std::string name) : name(name)
{
	Logger::setDestination(&stdDestination);
}

StdOutDestination Logger::stdDestination;
LogDestination *Logger::destination;
