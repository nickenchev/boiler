#ifndef FONTLOADER_H
#define FONTLOADER_H

#include "../core/component.h"

struct BMFont;

class FontLoader : public Component
{
public:
    FontLoader();

    const BMFont loadBMFont(const std::string fontPath) const;
};



#endif /* FONTLOADER_H */
