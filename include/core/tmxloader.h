#ifndef TMXLOADER_H
#define TMXLOADER_H

#include <memory>
#include <string>
#include "tmx.h"

namespace ensoft
{
	namespace tmx
	{
		class TmxLoader
		{
		public:
			static std::unique_ptr<ensoft::tmx::Map> loadMap(std::string filename);
		};
	};
};

#endif /* TMXLOADER_H */
