#include <iostream>
#include <SDL.h>

#include "engine.h"
#include "gamepart.h"
#include "spritesheet.h"
#include "spritesheetframe.h"

#include <map>
#include <zlib.h>
#include "tinyxml2.h"
#include "tmx.h"
#include "base64.h"
#include "string_util.h"

using namespace tinyxml2;

string getAttributeString(XMLElement *elem, const char *key)
{
    const char *str = elem->Attribute(key);
    return (str) ? string(str) : string("");
}

void loadData(std::string data)
{
    vector<BYTE> decoded = base64_decode(data);

    // TODO: look into better predicting the de-compressed size of the data
    const int compressionFactor = 1000;
    const unsigned long buffSize = decoded.size() * compressionFactor;

    uLong sourceLen = decoded.size();
    uLongf destLen = buffSize;
    Bytef buffer[buffSize];

    int result = uncompress(buffer, &destLen, &decoded[0], sourceLen);
    if (result != Z_OK)
    {
    }
    else
    {
        int offset = 0;
        do
        {
            u_int32_t value = *((u_int32_t *)&buffer[offset]);
            offset += 4;

            if (value != 0)
            {
                std::cout << value << " "; 
            }

        } while (offset < destLen);
    }

    std::cout << buffSize << " -> " << destLen << std::endl;
}

ensoft::Properties loadProperties(XMLElement *sourceElement)
{
    XMLElement *xprops = sourceElement->FirstChildElement("properties");
    std::map<string, ensoft::Property> props;
    if (xprops)
    {
        XMLElement *xprop = xprops->FirstChildElement("property");
        ensoft::Property prop;
        prop.name = getAttributeString(xprop, "name");
        prop.value = getAttributeString(xprop, "value");

        props.insert(std::pair<string, ensoft::Property>(prop.name, prop));
    }
    return ensoft::Properties(props);
}

void loadTmx()
{
    XMLDocument doc;
    XMLError error = doc.LoadFile("data/test_map.tmx");

    if (error != XMLError::XML_NO_ERROR)
    {
        std::cout << "Error code: " << error << std::endl;
    }
    else
    {
        // start parsing the TMX map
        XMLElement *xmap = doc.FirstChildElement("map");
        
        ensoft::Map map(loadProperties(xmap));
        map.version = xmap->Attribute("version");
        map.orientation = xmap->Attribute("orientation");
        map.width = xmap->IntAttribute("width");
        map.height = xmap->IntAttribute("height");
        map.tilewidth = xmap->IntAttribute("tilewidth");
        map.tileheight = xmap->IntAttribute("tileheight");
        map.renderorder = xmap->Attribute("renderorder");
        map.properties = loadProperties(xmap);

        // load all the tilesets
        XMLElement *xtileset = xmap->FirstChildElement("tileset");
        while (xtileset)
        {
            ensoft::TileSet tileSet(loadProperties(xtileset));
            tileSet.firstgid = xtileset->IntAttribute("firstgid");

            tileSet.source = getAttributeString(xtileset, "source");
            tileSet.name = xtileset->Attribute("name");
            tileSet.tilewidth = xtileset->IntAttribute("tilewidth");
            tileSet.tileheight = xtileset->IntAttribute("tileheight");
            tileSet.spacing = xtileset->IntAttribute("spacing");
            tileSet.margin = xtileset->IntAttribute("margin");
            map.tilesets.push_back(tileSet);

            // try to find another tileset
            xtileset = xtileset->NextSiblingElement("tileset");
        }

        XMLElement *xlayer = xmap->FirstChildElement("layer");
        while (xlayer)
        {
            ensoft::Layer layer(loadProperties(xlayer));
            layer.name = xlayer->Attribute("name");
            layer.x = xlayer->IntAttribute("x");
            layer.y = xlayer->IntAttribute("y");
            layer.width = xlayer->IntAttribute("width");
            layer.height = xlayer->IntAttribute("height");
            layer.opacity = xlayer->FloatAttribute("opacity");
            layer.visible = xlayer->BoolAttribute("visible");
            map.layers.push_back(layer);

            // load the data element
            XMLElement *xdata = xlayer->FirstChildElement("data");
            if (xdata)
            {
                string data = trim_copy(xdata->GetText());
                loadData(data);
            }
            
            xlayer = xlayer->NextSiblingElement("layer");
        }

        XMLElement *ximagelayer = xmap->FirstChildElement("imagelayer");
        while (ximagelayer)
        {
            ensoft::ImageLayer imageLayer(loadProperties(xlayer));
            imageLayer.name = ximagelayer->Attribute("name");
            imageLayer.x = ximagelayer->IntAttribute("x");
            imageLayer.y = ximagelayer->IntAttribute("y");
            imageLayer.width = ximagelayer->IntAttribute("width");
            imageLayer.height = ximagelayer->IntAttribute("height");
            imageLayer.opacity = ximagelayer->FloatAttribute("opacity");
            imageLayer.visible = ximagelayer->BoolAttribute("visible");
            map.imageLayers.push_back(imageLayer);

            ximagelayer = ximagelayer->NextSiblingElement("imagelayer");
        }

        XMLElement *xobjectgroup = xmap->FirstChildElement("objectgroup");
        while (xobjectgroup)
        {
            ensoft::ObjectGroup objectGroup(loadProperties(xobjectgroup));
            objectGroup.name = xobjectgroup->Attribute("name");
            objectGroup.x = xobjectgroup->IntAttribute("x");
            objectGroup.y = xobjectgroup->IntAttribute("y");
            objectGroup.width = xobjectgroup->IntAttribute("width");
            objectGroup.height = xobjectgroup->IntAttribute("height");
            objectGroup.opacity = xobjectgroup->FloatAttribute("opacity");
            objectGroup.visible = xobjectgroup->BoolAttribute("visible");

            //grab the objects
            XMLElement *xobject = xobjectgroup->FirstChildElement("object");
            while (xobject)
            {
                ensoft::Object object(loadProperties(xobject));
                object.id = xobject->Attribute("id");
                object.name = xobject->Attribute("name");
                object.type = getAttributeString(xobject, "type");
                object.x = xobject->IntAttribute("x");
                object.y = xobject->IntAttribute("y");
                object.width = xobject->IntAttribute("witdth");
                object.height = xobject->IntAttribute("height");
                object.gid = xobject->IntAttribute("gid");
                object.visible = xobject->BoolAttribute("visible");
                objectGroup.objects.push_back(object);

                xobject = xobject->NextSiblingElement("object");
            }
            map.objectGroups.push_back(objectGroup);

            xobjectgroup = xobjectgroup->NextSiblingElement("objectgroup");
        }
    }
}

void GamePart::start()
{
    //do some loading
    std::string filename = "data/zodas2.json";
    playerSheet = engine->getSpriteLoader().loadSheet(filename);

    //basic player setup
    player.frame.position.y = 200;
    frameNum = 1;
    numFrames = 2;
    animTime = 0;
    timePerFrame = 0.15f;

    walkLeft.push_back(playerSheet->getFrame("walk_left_01.png"));
    walkLeft.push_back(playerSheet->getFrame("walk_left_02.png"));
    walkRight.push_back(playerSheet->getFrame("walk_right_01.png"));
    walkRight.push_back(playerSheet->getFrame("walk_right_02.png"));

    currentAnimation = &walkRight;

    loadTmx();
}

void GamePart::update(const float delta)
{
    //animation stuff
    animTime += delta;
    if (animTime >= timePerFrame)
    {
        ++frameNum;
        if (frameNum > numFrames)
        {
            frameNum = 1;
        }
        animTime = 0;
    }
    frame = (*currentAnimation)[frameNum - 1];

    // check keyboard and modify state
    if (engine->keyState(SDLK_a))
    {
        currentAnimation = &walkLeft;
        player.frame.position.x -= 2;
    }
    else if (engine->keyState(SDLK_d))
    {
        currentAnimation = &walkRight;
        player.frame.position.x += 2;
    }
    else if (engine->keyState(SDLK_ESCAPE))
    {
        engine->quit();
    }
}

void GamePart::render()
{
    SDL_Rect sourceRect;
    sourceRect.x = frame->getSourceRect().position.x;
    sourceRect.y = frame->getSourceRect().position.y;
    sourceRect.w = frame->getSourceRect().size.width;
    sourceRect.h = frame->getSourceRect().size.height;

    SDL_Rect destRect;
    destRect.x = player.frame.position.x;
    destRect.y = player.frame.position.y;
    destRect.w = frame->getSourceRect().size.width;
    destRect.h = frame->getSourceRect().size.height; 

    double angle = frame->isRotated() ? -90 : 0;
    SDL_RenderCopyEx(engine->getRenderer(), playerSheet->getTexture(), &sourceRect, &destRect, angle, NULL, SDL_FLIP_NONE);
}
