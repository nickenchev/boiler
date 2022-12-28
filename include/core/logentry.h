#ifndef LOGENTRY_H
#define LOGENTRY_H

#include <ctime>
#include <string>
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
        timestamp = std::time(nullptr);
		this->level = level;
		this->source = source;
		this->message = message;
	}

	std::time_t getTimestamp() const { return timestamp; }
	LogLevel getLevel() const { return level; }
	std::string getSource() const { return source; }
	std::string getMessage() const { return message; }
};

}

#endif /* LOGENTRY_H */
