#include <map>
#include <zlib.h>
#include <iostream>
#include <glm/glm.hpp>
#include "tmxloader.h"
#include "tinyxml2.h"
#include "base64.h"
#include "../util/string_util.h"

using namespace tinyxml2;

string getAttributeString(XMLElement *elem, const char *key)
{
    const char *str = elem->Attribute(key);
    return (str) ? string(str) : string("");
}

ensoft::Image loadImage(XMLElement *element)
{
    XMLElement *ximage = element->FirstChildElement("image");
    std::string format = getAttributeString(ximage, "format");
    std::string source = getAttributeString(ximage, "source");
    std::string trans = getAttributeString(ximage, "trans");
    int width = ximage->IntAttribute("width");
    int height = ximage->IntAttribute("height");

    return ensoft::Image{format, source, trans, width, height};
}

void loadData(ensoft::Layer &layer, ensoft::Map *map, std::string data)
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
        // TODO: Error handle
    }
    else
    {
        int offset = 0;
        do
        {
            u_int32_t tileGid = *((u_int32_t *)&buffer[offset]);
            layer.tiles.push_back(tileGid);

            offset += 4;
        } while (offset < destLen);
    }
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

std::unique_ptr<ensoft::Map> TmxLoader::loadMap(std::string filename)
{
    XMLDocument doc;
    XMLError error = doc.LoadFile(filename.c_str());
    std::unique_ptr<ensoft::Map> map;

    if (error != XMLError::XML_NO_ERROR)
    {
        std::cout << "Error code: " << error << std::endl;
    }
    else
    {
        // start parsing the TMX map
        XMLElement *xmap = doc.FirstChildElement("map");
        
        map = std::make_unique<ensoft::Map>(loadProperties(xmap));
        map->version = xmap->Attribute("version");
        map->orientation = xmap->Attribute("orientation");
        map->width = xmap->IntAttribute("width");
        map->height = xmap->IntAttribute("height");
        map->tilewidth = xmap->IntAttribute("tilewidth");
        map->tileheight = xmap->IntAttribute("tileheight");
        map->renderorder = xmap->Attribute("renderorder");
        map->properties = loadProperties(xmap);

        // load all the tilesets
        XMLElement *xtileset = xmap->FirstChildElement("tileset");
        while (xtileset)
        {
            auto tileSet = std::make_unique<ensoft::TmxTileSet>(loadProperties(xtileset));
            tileSet->firstgid = xtileset->IntAttribute("firstgid");
            tileSet->source = getAttributeString(xtileset, "source");
            tileSet->name = xtileset->Attribute("name");
            tileSet->tilewidth = xtileset->IntAttribute("tilewidth");
            tileSet->tileheight = xtileset->IntAttribute("tileheight");
            tileSet->spacing = xtileset->IntAttribute("spacing");
            tileSet->margin = xtileset->IntAttribute("margin");

            // get the tiles for this tileset
            XMLElement *xtile = xtileset->FirstChildElement("tile");
            while (xtile)
            {
                auto tile = make_unique<ensoft::TmxTile>(loadProperties(xtile));
                tile->id = xtile->IntAttribute("id");
                tile->terrain = getAttributeString(xtile, "terrain");
                tile->probability = xtile->FloatAttribute("probability");
                tile->image = loadImage(xtile);

                // keep track of all tiles and their global IDs
                int tileGid = tileSet->firstgid + tile->id;
                map->allTiles[tileGid] = tile.get();
                std::cout << "[TMX Loader] Added: " << tile->image.source << " GID: " << tileGid << " Tileset: " << tileSet->name << std::endl;

                tileSet->tiles.push_back(std::move(tile));
                xtile = xtile->NextSiblingElement("tile");
            }

            map->tilesets.push_back(std::move(tileSet));
            // try to find another tileset
            xtileset = xtileset->NextSiblingElement("tileset");
        }

        XMLElement *xlayer = xmap->FirstChildElement("layer");
        while (xlayer)
        {
            auto layer = std::make_shared<ensoft::Layer>(loadProperties(xlayer));
            layer->name = xlayer->Attribute("name");
            layer->x = xlayer->IntAttribute("x");
            layer->y = xlayer->IntAttribute("y");
            layer->width = xlayer->IntAttribute("width");
            layer->height = xlayer->IntAttribute("height");
            layer->opacity = xlayer->FloatAttribute("opacity");
            layer->visible = xlayer->BoolAttribute("visible");

            // load the data element
            XMLElement *xdata = xlayer->FirstChildElement("data");
            if (xdata)
            {
                string data = trim_copy(xdata->GetText());
                loadData(*layer, map.get(), data);
            }
            
            map->layers.push_back(layer);
            xlayer = xlayer->NextSiblingElement("layer");
        }

        XMLElement *ximagelayer = xmap->FirstChildElement("imagelayer");
        while (ximagelayer)
        {
            auto imageLayer = std::make_unique<ensoft::ImageLayer>(loadProperties(ximagelayer));
            imageLayer->name = ximagelayer->Attribute("name");
            imageLayer->x = ximagelayer->IntAttribute("x");
            imageLayer->y = ximagelayer->IntAttribute("y");
            imageLayer->width = ximagelayer->IntAttribute("width");
            imageLayer->height = ximagelayer->IntAttribute("height");
            imageLayer->opacity = ximagelayer->FloatAttribute("opacity");
            imageLayer->visible = ximagelayer->BoolAttribute("visible");

            imageLayer->image = loadImage(ximagelayer);
            map->imageLayers.push_back(std::move(imageLayer));

            ximagelayer = ximagelayer->NextSiblingElement("imagelayer");
        }

        XMLElement *xobjectgroup = xmap->FirstChildElement("objectgroup");
        while (xobjectgroup)
        {
            auto objectGroup = std::make_unique<ensoft::ObjectGroup>(loadProperties(xobjectgroup));
            objectGroup->name = xobjectgroup->Attribute("name");
            objectGroup->x = xobjectgroup->IntAttribute("x");
            objectGroup->y = xobjectgroup->IntAttribute("y");
            objectGroup->width = xobjectgroup->IntAttribute("width");
            objectGroup->height = xobjectgroup->IntAttribute("height");
            objectGroup->opacity = xobjectgroup->FloatAttribute("opacity");
            objectGroup->visible = xobjectgroup->BoolAttribute("visible");

            //grab the objects
            XMLElement *xobject = xobjectgroup->FirstChildElement("object");
            while (xobject)
            {
                auto object = std::make_unique<ensoft::Object>(loadProperties(xobject));
                object->id = xobject->Attribute("id");
                object->name = getAttributeString(xobject, "name");
                object->type = getAttributeString(xobject, "type");
                object->x = xobject->IntAttribute("x");
                object->y = xobject->IntAttribute("y");
                object->width = xobject->IntAttribute("witdth");
                object->height = xobject->IntAttribute("height");
                object->gid = xobject->IntAttribute("gid");
                object->visible = xobject->BoolAttribute("visible");
                objectGroup->objects.push_back(std::move(object));

                xobject = xobject->NextSiblingElement("object");
            }
            map->objectGroups.push_back(std::move(objectGroup));
            xobjectgroup = xobjectgroup->NextSiblingElement("objectgroup");
        }
    }

    return std::move(map); 
}
