#ifndef MAPLOADER_H
#define MAPLOADER_H

#include <string>
#include "core/engine.h"
#include "core/logger.h"

namespace Boiler
{
	class MapLoader
	{
		Logger logger;
		Engine &engine;

	public:
		MapLoader(Engine &engine) : logger("Map Loader"), engine(engine) { }

		void load(const std::string &filePath);
	};
}

#endif