#include "core/logentry.h"
#include <string>
#include <iostream>
#include <iomanip>

#include "core/logdestination.h"

using namespace Boiler;

void LogDestination::log(const LogEntry &entry)
{
	std::string level;
	switch (entry.getLevel())
	{
		case LogLevel::INFO:
		{
			level = "INFO";
			break;
		}
		case LogLevel::WARNING:
		{
			level = "WARN";
			break;
		}
		case LogLevel::ERROR:
		{
			level = "ERROR";
			break;
		}
	}
	std::cout << std::left << std::setw(10) << level
			  << std::setw(28) << entry.getSource()
			  << entry.getMessage() << std::endl;
}


const std::vector<LogEntry> &Boiler::LogDestination::getEntries() const
{
	return entries;
}