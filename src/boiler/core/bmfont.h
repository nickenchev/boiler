#ifndef BMFONT_H
#define BMFONT_H

#include <map>
#include <vector>
#include <string>
#include <memory>
#include "rect.h"

class Texture;

struct Page
{
    int id;
    std::string file;
    std::shared_ptr<Texture> texture;
};

struct Char
{
    int id, xoffset, yoffset, xadvance, chnl;
    Rect frame;
    std::string letter;
    const Page &page;

    Char(const Page &page) : page(page) { }
};

struct BMFont
{
    std::string face, charset, padding, spacing;
    int size, stretchH, lineHeight, base, scaleW, scaleH, pages;
    bool bold, italic, unicode, smooth, aa, packed;

    std::vector<Page> pageList;
    std::vector<Char> charList;
    std::map<char, const Char *> charMap;

    BMFont();
};

#endif /* BMFONT_H */
