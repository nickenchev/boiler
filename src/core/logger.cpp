#include "core/logger.h"
#include "core/logdestination.h"
#include "core/logentry.h"
#include "core/stdoutdestination.h"
#include <iomanip>
#include <string>

using namespace Boiler;

Logger::Logger(std::string name) : name(name)
{
}

void Logger::printEntry(const LogEntry &entry) const
{
	assert(destination != nullptr);
	
	destination->log(entry);
	// std::string level;
	// switch (entry.getLevel())
	// {
	// 	case LogLevel::INFO:
	// 	{
	// 		level = "INFO";
	// 		break;
	// 	}
	// 	case LogLevel::WARNING:
	// 	{
	// 		level = "WARN";
	// 		break;
	// 	}
	// 	case LogLevel::ERROR:
	// 	{
	// 		level = "ERROR";
	// 		break;
	// 	}
	// }
	// std::cout << std::left << std::setw(10) << level
	// 		  << std::setw(28) << entry.getSource()
	// 		  << entry.getMessage() << std::endl;
}

LogDestination* Logger::destination;
