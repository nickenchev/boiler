#ifndef FONTLOADER_H
#define FONTLOADER_H

#include "component.h"

class BMFont;

class FontLoader : public Component
{
public:
    FontLoader(Engine &engine);

    const BMFont loadBMFont(const std::string fontPath) const;
};



#endif /* FONTLOADER_H */
