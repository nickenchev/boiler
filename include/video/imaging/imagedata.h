#ifndef IMAGEDATA_H
#define IMAGEDATA_H

#include "core/rect.h"

namespace Boiler
{
	struct ImageData
	{
		unsigned char * pixelData;
		Size size;
		short colorComponents;

		ImageData(unsigned char *pixelData, const Size &size, short colorComponents) : pixelData(pixelData), size(size)
		{
			this->colorComponents = colorComponents;
		}

		ImageData(ImageData &&imageData)
		{
			this->pixelData = imageData.pixelData;
			this->size = imageData.size;
			this->colorComponents = imageData.colorComponents;

			imageData.pixelData = nullptr;
		}

		~ImageData()
		{
			if (pixelData)
			{
				free(pixelData);
			}
		}
	};
}

#endif /* IMAGEDATA_H */
