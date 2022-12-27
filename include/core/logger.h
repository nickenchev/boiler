#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <vector>
#include <iostream>
#include <utility>
#include <fmt/core.h>
#include <chrono>

namespace Boiler
{

enum class LogLevel
{
	INFO, WARNING, ERROR
};

class LogEntry
{
	std::time_t timestamp;
	LogLevel level;
	std::string source, message;

public:
	LogEntry(LogLevel level, std::string source, std::string message)
	{
		this->source = source;
		this->message = message;
	}
};

class Logger
{
	const std::string name;

public:
    Logger(std::string name);

	template<typename... Args>
	LogEntry log(fmt::format_string<Args...> fmtString, Args&&... args) const
	{
		return LogEntry(LogLevel::INFO, name, fmt::format(fmtString, std::forward<Args>(args)...));
	}

	template<typename... Args>
	LogEntry error(fmt::format_string<Args...> fmtString, Args&&... args) const
	{
		return LogEntry(LogLevel::ERROR, name, fmt::format(fmtString, std::forward<Args>(args)...));
	}
};

}

#endif /* LOGGER_H */
