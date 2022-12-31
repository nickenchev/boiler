#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <vector>
#include <iostream>
#include <utility>
#include <fmt/core.h>
#include "core/logentry.h"
#include "core/stdoutdestination.h"

namespace Boiler
{

class Logger
{
	static StdOutDestination stdDestination;
	static LogDestination *destination;
	const std::string name;

public:
    Logger(std::string name);

	template<typename... Args>
	void log(fmt::format_string<Args...> fmtString, Args&&... args) const
	{
		LogEntry entry(LogLevel::INFO, name, fmt::format(fmtString, std::forward<Args>(args)...));
		Logger::getDestination()->log(entry);
	}

	template<typename... Args>
	void error(fmt::format_string<Args...> fmtString, Args&&... args) const
	{
		LogEntry entry(LogLevel::ERROR, name, fmt::format(fmtString, std::forward<Args>(args)...));
		Logger::getDestination()->log(entry);
	}

	static LogDestination *getDestination()
	{
		return destination;
	}
	static void setDestination(LogDestination *dest)
	{
		Logger::destination = dest;
	}
};

}

#endif /* LOGGER_H */
