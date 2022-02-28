#ifndef JPEGLOADER_H
#define JPEGLOADER_H

#include "display/imaging/imageloader.h"

namespace Boiler
{
	struct ImageData;
	
	class JPEGLoader : public ImageLoader
	{
		ImageData readImage(const std::string &filePath) const override;
	};

}

#endif /* JPEGLOADER_H */
