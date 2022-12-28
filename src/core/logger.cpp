#include "core/logger.h"
#include "core/logdestination.h"

using namespace Boiler;

Logger::Logger(std::string name) : name(name)
{
}

LogDestination *Logger::destination;
