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
		AssetSet &assetSet;

	public:
		MapLoader(AssetSet &assetSet, Engine &engine) : logger("Map Loader"), engine(engine), assetSet(assetSet) { }

		void load(const std::string &filePath);
	};
}

#endif
