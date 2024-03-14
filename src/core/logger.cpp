#include <cassert>

#include "core/logger.h"
#include "core/logdestination.h"
#include "core/logentry.h"

using namespace Boiler;

Logger::Logger(std::string name) : name(name)
{

}

void Logger::printEntry(const LogEntry &entry) const
{
	assert(destination != nullptr);
	destination->log(entry);
}

LogDestination* Logger::destination = nullptr;
