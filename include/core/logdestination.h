#ifndef LOGDESTINATION_H
#define LOGDESTINATION_H

#include <vector>
#include "core/logentry.h"

namespace Boiler
{

class LogEntry;

class LogDestination
{
	std::vector<LogEntry> entries;

public:
	virtual void log(const LogEntry &entry);
	virtual const std::vector<LogEntry> &getEntries() const;
};

}

#endif /* LOGDESTINATION_H */
