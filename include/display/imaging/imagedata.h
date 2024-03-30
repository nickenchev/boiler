#ifndef IMAGEDATA_H
#define IMAGEDATA_H

#include <string>
#include "core/rect.h"

namespace Boiler
{
	struct ImageData
	{
	private:

		ImageData &operator=(const ImageData &id)
		{
			pixelData = id.pixelData;
			size = id.size;
			colorComponents = id.colorComponents;
			hasAlpha = id.hasAlpha;
			return *this;
		}

	public:
		const std::string sourcePath;
		unsigned char *pixelData;
		Size size;
		short colorComponents;
		bool hasAlpha;

		ImageData()
		{
			pixelData = nullptr;
			colorComponents = 0;
			hasAlpha = false;
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
			*this = imageData;
		}

		ImageData(ImageData &&imageData)
		{
			*this = imageData;
			imageData.pixelData = nullptr;
		}

		~ImageData()
		{
			if (pixelData)
			{
				delete [] pixelData;
				pixelData = nullptr;
			}
		}

		ImageData &operator=(ImageData &&id)
		{
			*this = id;

			id.pixelData = nullptr;
			return *this;
		}

		const std::string &getSourcePath() const { return sourcePath; }
	};
}

#endif /* IMAGEDATA_H */
