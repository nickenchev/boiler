#ifndef BMFONT_H
#define BMFONT_H

#include <map>
#include <vector>
#include <string>
#include "rect.h"

struct Page
{
    int id;
    std::string file;
};

struct Char
{
    int id, xoffset, yoffset, xadvance, chnl;
    Rect frame;
    std::string letter;
    const Page &page;

    Char(const Page &page) : page(page) { }
};

class BMFont
{
    std::string face, charset, padding, spacing;
    int size, stretchH, lineHeight, base, scaleW, scaleH, pages;
    bool bold, italic, unicode, smooth, aa, packed;

    std::vector<Page> pageList;
    std::vector<Char> charList;
    std::map<char, const Char *> charMap;

    void loadInfo(std::string data);

public:
    BMFont(std::string filename);
};

#endif /* BMFONT_H */
