#ifndef FONTLOADER_H
#define FONTLOADER_H

#include "core/logger.h"

struct BMFont;

class FontLoader
{
	Logger logger;
	
public:
    FontLoader();

    const BMFont loadBMFont(const std::string fontPath) const;
};



#endif /* FONTLOADER_H */
