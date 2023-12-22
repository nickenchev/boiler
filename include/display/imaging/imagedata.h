#ifndef IMAGEDATA_H
#define IMAGEDATA_H

#include "core/rect.h"

namespace Boiler
{
	struct ImageData
	{
		unsigned char *pixelData;
		Size size;
		short colorComponents;
		bool hasAlpha;

		ImageData(unsigned char *pixelData, const Size &size, short colorComponents) : size(size)
		{
			const size_t byteSize = (size.width * size.height) * colorComponents;
			this->pixelData = new unsigned char[byteSize];
			std::copy(pixelData, pixelData + byteSize, this->pixelData);

			this->colorComponents = colorComponents;
			this->hasAlpha = colorComponents > 3;
		}

		ImageData(ImageData &&imageData)
		{
			this->pixelData = imageData.pixelData;
			this->size = imageData.size;
			this->colorComponents = imageData.colorComponents;
			this->hasAlpha = imageData.hasAlpha;

			imageData.pixelData = nullptr;
		}

		~ImageData()
		{
			if (pixelData)
			{
				delete [] pixelData;
			}
		}
	};
}

#endif /* IMAGEDATA_H */
