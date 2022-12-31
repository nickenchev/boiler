#ifndef STDOUTDESTINATION_H
#define STDOUTDESTINATION_H

#include <iostream>
#include <vector>
#include "core/logdestination.h"
#include "core/logentry.h"

namespace Boiler
{

class StdOutDestination : public LogDestination
{
	std::vector<LogEntry> entries;
public:
    StdOutDestination() {}
	
	void log(const LogEntry &entry) override
	{
		std::cout << "[" << entry.getSource() << "] " << entry.getMessage() << std::endl;
	}
	const std::vector<LogEntry> &getEntries() const override
	{
		return entries;
	}
};

}

#endif /* STDOUTDESTINATION_H */
