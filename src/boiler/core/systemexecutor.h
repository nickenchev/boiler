#ifndef SYSTEMEXECUTOR_H
#define SYSTEMEXECUTOR_H

#include <vector>
#include "system.h"
#include "componentmapper.h"

class SystemExecutor
{
	ComponentMapper componentMapper;
	std::vector<System> systems;
	
public:
    SystemExecutor();
};


#endif /* SYSTEMEXECUTOR_H */
