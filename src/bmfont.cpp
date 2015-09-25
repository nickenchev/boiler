#include <regex>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include "bmfont.h"
#include "tinyxml2.h"

using namespace tinyxml2;

BMFont::BMFont(std::string filename)
{
    XMLDocument doc;
    XMLError error = doc.LoadFile(filename.c_str());

    if (error != XMLError::XML_NO_ERROR)
    {
        std::cerr << "Error loading fnt file (XML)" << std::endl;
    }
    else
    {
        XMLElement *fontElem = doc.FirstChildElement("font");
        if (fontElem)
        {
            // load info
            XMLElement *infoElem = fontElem->FirstChildElement("info");
            face = infoElem->Attribute("face");
            size = infoElem->IntAttribute("size");
            bold = infoElem->BoolAttribute("bold");
            italic = infoElem->BoolAttribute("italic");
            charset = infoElem->Attribute("chasrset"); // type exists in source file
            unicode = infoElem->BoolAttribute("unicode");
            stretchH = infoElem->IntAttribute("stretchH");
            smooth = infoElem->BoolAttribute("smooth");
            aa = infoElem->BoolAttribute("aa");
            padding = infoElem->Attribute("padding");
            spacing = infoElem->Attribute("spacing");

            // load common
            XMLElement *commonElem = fontElem->FirstChildElement("common");
            lineHeight = commonElem->IntAttribute("lineHeight");
            base = commonElem->IntAttribute("base");
            scaleW = commonElem->IntAttribute("scaleW");
            scaleH = commonElem->IntAttribute("scaleH");
            pages = commonElem->IntAttribute("pages");
            packed = commonElem->BoolAttribute("packed");

            //load pages
            XMLElement *pagesElem = fontElem->FirstChildElement("pages");
            XMLElement *page = pagesElem->FirstChildElement("page");
            while (page)
            {
                Page pageObj;
                pageObj.id = page->IntAttribute("id");
                pageObj.file = page->Attribute("file");
                pageList.push_back(std::move(pageObj));

                page = page->NextSiblingElement();
            }

            // load chars
            XMLElement *charsElem = fontElem->FirstChildElement("chars");
            XMLElement *charElem = charsElem->FirstChildElement("char");
            while (charElem)
            {
                int pageId = charElem->IntAttribute("page");

                Char charObj(pageList[pageId]);
                charObj.id = charElem->IntAttribute("id");
                charObj.frame.position.x = charElem->IntAttribute("x");
                charObj.frame.position.y = charElem->IntAttribute("y");
                charObj.frame.size.setWidth(charElem->IntAttribute("width"));
                charObj.frame.size.setHeight(charElem->IntAttribute("height"));
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
                charList.push_back(std::move(charObj));

                // finally map the letter to the charmap
                const Char *charPtr = &charList[charList.size() - 1];
                charMap.insert(std::make_pair(letter, charPtr));
                
                charElem = charElem->NextSiblingElement();
            }
        }
    }
}

void BMFont::loadInfo(std::string data)
{
}
