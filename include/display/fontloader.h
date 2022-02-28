#ifndef FONTLOADER_H
#define FONTLOADER_H

#include "core/logger.h"

namespace Boiler
{

struct BMFont;
class ImageLoader;

class FontLoader
{
	Logger logger;
	const ImageLoader &imageLoader;
	
public:
    FontLoader(const ImageLoader &imageLoader);

	const BMFont loadBMFont(const std::string fontPath) const;
};

}
#endif /* FONTLOADER_H */
