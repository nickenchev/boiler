#ifndef PNGLOADER_H
#define PNGLOADER_H

#include <string>
#include "video/imaging/imageloader.h"

namespace Boiler
{
	class PNGLoader : public ImageLoader
	{
		ImageData readImage(const std::string &filePath) const override;
	};
}

#endif /* PNGLOADER_H */
