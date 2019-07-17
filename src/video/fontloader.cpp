#include <regex>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include "core/engine.h"
#include "core/bmfont.h"
#include "core/fileutils.h"
#include "video/fontloader.h"
#include "video/imageloader.h"
#include "tinyxml2/tinyxml2.h"

#define COMPONENT_NAME "FontLoader"

using namespace Boiler;
using namespace tinyxml2;

FontLoader::FontLoader(const ImageLoader &imageLoader) : logger(COMPONENT_NAME), imageLoader(imageLoader)
{
}

const BMFont FontLoader::loadBMFont(const std::string fontPath) const
{
    XMLDocument doc;
    XMLError error = doc.LoadFile(fontPath.c_str());

    BMFont font;
    if (error != XMLError::XML_NO_ERROR)
    {
        std::cerr << "Error loading fnt file (XML)" << std::endl;
    }
    else
    {
        logger.log("Loading font file: " + fontPath);
        std::string imagePath = FileUtils::getDirectory(fontPath);
        logger.log("Using: \"" + imagePath + "\" directory for page images.");

        XMLElement *fontElem = doc.FirstChildElement("font");
        if (fontElem)
        {
            // load info
            XMLElement *infoElem = fontElem->FirstChildElement("info");
            font.face = infoElem->Attribute("face");
            font.size = infoElem->IntAttribute("size");
            font.bold = infoElem->BoolAttribute("bold");
            font.italic = infoElem->BoolAttribute("italic");
            font.charset = infoElem->Attribute("chasrset"); // type exists in source file
            font.unicode = infoElem->BoolAttribute("unicode");
            font.stretchH = infoElem->IntAttribute("stretchH");
            font.smooth = infoElem->BoolAttribute("smooth");
            font.aa = infoElem->BoolAttribute("aa");
            font.padding = infoElem->Attribute("padding");
            font.spacing = infoElem->Attribute("spacing");

            // load common
            XMLElement *commonElem = fontElem->FirstChildElement("common");
            font.lineHeight = commonElem->IntAttribute("lineHeight");
            font.base = commonElem->IntAttribute("base");
            font.scaleW = commonElem->IntAttribute("scaleW");
            font.scaleH = commonElem->IntAttribute("scaleH");
            font.pages = commonElem->IntAttribute("pages");
            font.packed = commonElem->BoolAttribute("packed");

            //load pages
            XMLElement *pagesElem = fontElem->FirstChildElement("pages");
            XMLElement *page = pagesElem->FirstChildElement("page");
            while (page)
            {
                Page pageObj;
                pageObj.id = page->IntAttribute("id");
                pageObj.file = page->Attribute("file");
                pageObj.texture = imageLoader.loadImage(FileUtils::buildPath(imagePath, pageObj.file));
                font.pageList.push_back(std::move(pageObj));

                page = page->NextSiblingElement();
            }

            // load chars
            XMLElement *charsElem = fontElem->FirstChildElement("chars");
            XMLElement *charElem = charsElem->FirstChildElement("char");
            while (charElem)
            {
                int pageId = charElem->IntAttribute("page");

                Char charObj(font.pageList[pageId]);
                charObj.id = charElem->IntAttribute("id");
                charObj.frame.position.x = charElem->IntAttribute("x");
                charObj.frame.position.y = charElem->IntAttribute("y");
                charObj.frame.size.width = charElem->IntAttribute("width");
                charObj.frame.size.height = charElem->IntAttribute("height");
                charObj.xoffset = charElem->IntAttribute("xoffset");
                charObj.yoffset = charElem->IntAttribute("yoffset");
                charObj.xadvance = charElem->IntAttribute("xadvance");
                charObj.chnl = charElem->IntAttribute("chnl");
                charObj.letter = charElem->Attribute("letter");

                // add this to the char map
                char letter;
                if (charObj.letter.compare("space") == 0)
                {
                    letter = ' ';
                }
                else if (charObj.letter.compare("&quot;") == 0)
                {
                    letter = '\'';
                }
                else if (charObj.letter.compare("&amp;") == 0)
                {
                    letter = '&';
                }
                else if (charObj.letter.compare("&lt;") == 0)
                {
                    letter = '<';
                }
                else if (charObj.letter.compare("&gt;") == 0)
                {
                    letter = '>';
                }
                else if (charObj.letter.size() == 1)
                {
                    letter = charObj.letter[0];
                }
                font.charList.push_back(std::move(charObj));

                // finally map the letter to the charmap
                const Char *charPtr = &font.charList[font.charList.size() - 1];
                font.charMap.insert(std::make_pair(letter, charPtr));
                
                charElem = charElem->NextSiblingElement();
            }
        }
    }
    return font;
}
