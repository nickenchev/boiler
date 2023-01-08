#ifndef LOGDESTINATION_H
#define LOGDESTINATION_H

#include <vector>

namespace Boiler
{

class LogEntry;

class LogDestination
{
public:
	virtual void log(const LogEntry &entry);
	virtual const std::vector<LogEntry> &getEntries() const = 0;
};

}

#endif /* LOGDESTINATION_H */
