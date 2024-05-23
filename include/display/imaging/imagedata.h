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
			sourcePath = id.sourcePath;
			pixelData = id.pixelData;
			size = id.size;
			colorComponents = id.colorComponents;
			hasAlpha = id.hasAlpha;
			return *this;
		}

	public:
		std::string sourcePath;
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

		ImageData(const std::string &sourcePath, unsigned char *pixelData, const Size &size, short colorComponents)
		{
			this->sourcePath = sourcePath;
			this->size = size;
			this->colorComponents = colorComponents;
			this->hasAlpha = colorComponents > 3;

			this->pixelData = new unsigned char[byteSize()];
			std::copy(pixelData, pixelData + byteSize(), this->pixelData);
		}

		ImageData(const ImageData &imageData)
		{
			*this = imageData;
		}

		ImageData(ImageData &&imageData) noexcept
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

		ImageData &operator=(ImageData &&id) noexcept
		{
			// delete existing pixel data
			if (pixelData)
			{
				delete[] pixelData;
			}
			*this = id;

			id.pixelData = nullptr;
			return *this;
		}

		const std::string &getSourcePath() const { return sourcePath; }
		unsigned int byteSize() const { return (size.width * size.height) * colorComponents; }
	};
}

#endif /* IMAGEDATA_H */
