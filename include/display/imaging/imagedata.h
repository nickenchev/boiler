#ifndef IMAGEDATA_H
#define IMAGEDATA_H

#include <string>
#include "core/rect.h"

namespace Boiler
{
	struct ImageData
	{
		const std::string sourcePath;
		unsigned char *pixelData;
		Size size;
		short colorComponents;
		bool hasAlpha;

		ImageData()
		{
			pixelData = nullptr;
		}

		ImageData(const std::string &sourcePath, unsigned char *pixelData, const Size &size, short colorComponents) : sourcePath(sourcePath), size(size)
		{
			const size_t byteSize = (size.width * size.height) * colorComponents;
			this->pixelData = new unsigned char[byteSize];
			std::copy(pixelData, pixelData + byteSize, this->pixelData);

			this->colorComponents = colorComponents;
			this->hasAlpha = colorComponents > 3;
		}

		ImageData(const ImageData &imageData) : sourcePath(imageData.sourcePath)
		{
			this->pixelData = imageData.pixelData;
			this->size = imageData.size;
			this->colorComponents = imageData.colorComponents;
			this->hasAlpha = imageData.hasAlpha;
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

		const std::string &getSourcePath() const { return sourcePath; }
	};
}

#endif /* IMAGEDATA_H */
