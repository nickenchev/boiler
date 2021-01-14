#include <array>

#include "core/engine.h"
#include "video/skyboxloader.h"
#include "video/imaging/imageloader.h"

using namespace Boiler;

void SkyBoxLoader::load(const std::string &top, const std::string &bottom,
						const std::string &left, const std::string &right,
						const std::string &front, const std::string &back)
{
	std::array<ImageData, 6> images{
		ImageLoader::load(top),
		ImageLoader::load(bottom),
		ImageLoader::load(left),
		ImageLoader::load(right),
		ImageLoader::load(front),
		ImageLoader::load(back)
	};
}
