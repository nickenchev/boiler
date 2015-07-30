#include <iostream>
#include <SDL.h>

#include "engine.h"
#include "gamepart.h"
#include "spritesheet.h"
#include "spritesheetframe.h"

#include "sdl_util.h" //TODO: Meh?

// LOAD TMX Stuff
#define GLM_COMPILER 0
#include <map>
#include <zlib.h>
#include "tinyxml2.h"
#include "base64.h"
#include "string_util.h"
#include <glm/glm.hpp>

// OpenGL Rendering
#include "opengl.h"
#include <glm/gtc/matrix_transform.hpp>

using namespace tinyxml2;
using namespace std;

GamePart::GamePart(Engine *engine) : Part(engine)
{
}

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
    std::vector<u_int32_t> tileList;
    std::map<int, ensoft::TmxTile *> tilesUsed;
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

std::unique_ptr<ensoft::Map> loadTmx()
{
    XMLDocument doc;
    XMLError error = doc.LoadFile("data/test_map.tmx");
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
            auto layer = std::make_unique<ensoft::Layer>(loadProperties(xlayer));
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
            
            map->layers.push_back(std::move(layer));
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

    return map; 
}

void GamePart::start()
{
    //do some loading
    playerSheet = engine->getSpriteLoader().loadSheet("data/zodas2.json");
    tilesSheet = engine->getSpriteLoader().loadSheet("data/tiles.json");

    //basic player setup
    player.frame.position = glm::vec2(20, 300);
    player.frame.size = Size(15, 31);
    frameNum = 1;
    numFrames = 2;
    animTime = 0;
    timePerFrame = 0.15f;

    walkLeft.push_back(playerSheet->getFrame("walk_left_01.png"));
    walkLeft.push_back(playerSheet->getFrame("walk_left_02.png"));
    walkRight.push_back(playerSheet->getFrame("walk_right_01.png"));
    walkRight.push_back(playerSheet->getFrame("walk_right_02.png"));

    currentAnimation = &walkRight;

    tmxMap = std::move(loadTmx());
    // create the entities from the tile info

    int tileNum = 0;
    int x = 0, y = 0;
    for (int w = 0; w < tmxMap->width; ++w)
    {
        for (int h = 0; h < tmxMap->height; ++h)
        {
            int tileGid = tmxMap->layers[0]->tiles[tileNum];
            if (tileGid)
            {
                const ensoft::TmxTile *tmxTile= tmxMap->allTiles[tileGid];
                Tile tile(*tmxTile);
                tile.frame = Rect(x, y, tmxMap->tilewidth, tmxMap->tileheight);
                tiles.push_back(std::move(tile));
            }

            x += tmxMap->tilewidth;
            tileNum++;
        }
        y += tmxMap->tileheight;
        x = 0;
    }

    // 2D vertex and texture coords
    GLfloat sizeW = player.frame.size.width;
    GLfloat sizeH = player.frame.size.height;

    GLfloat vertices[] =
    {
        0.0f, sizeH,
        sizeW, 0.0f,
        0.0f, 0.0f,

        0.0f, sizeH,
        sizeW, sizeH,
        sizeW, 0.0f
    };

    program = std::make_unique<ShaderProgram>("shader");

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // setup a VBO for the vertices
    glGenBuffers(1, &vboVerts);
    glBindBuffer(GL_ARRAY_BUFFER, vboVerts);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);

    //enable this attrib array for the tex VBOs
    glEnableVertexAttribArray(1);

    // cleanup
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    mvpUniform = glGetUniformLocation(program->getShaderProgram(), "MVP");
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
    spriteFrame = (*currentAnimation)[frameNum - 1];

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
    glUseProgram(program->getShaderProgram());

    // set the vao for the current sprite
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, spriteFrame->getTexCoordsVbo());
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);

    // opengl sprite render
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(getEngine()->getScreenWidth()),
                                      static_cast<GLfloat>(getEngine()->getScreenHeight()), 0.0f, -1.0f, 1.0f);

    // create the model matrix, by getting a 3D vector from the Entity's vec2 position
    glm::mat4 model = glm::translate(glm::mat4(1), glm::vec3(player.frame.position, 0.0f));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    projection = projection * model;

    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, &projection[0][0]);

    // the sprite becomes TEXTURE0 in the shader
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, playerSheet->getTexture());

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glUseProgram(0);
    // draw the player
//    SDL_Rect sourceRect = make_rect(frameRect);
//    SDL_Rect destRect = make_rect(player.frame.position, frameRect.size);
//
//    double angle = spriteFrame->isRotated() ? -90 : 0;
//    SDL_RenderCopyEx(engine->getRenderer(), playerSheet->getTexture(), &sourceRect, &destRect, angle, NULL, SDL_FLIP_NONE);
//
//    // draw the tiles
//    for (const auto &tile : tiles)
//    {
//        const SpriteSheetFrame *tileFrame = tilesSheet->getFrame(tile.getTmxTile().image.source);
//
//        SDL_Rect srect = make_rect(tileFrame->getSourceRect().position, tileFrame->getSourceRect().size);
//        SDL_Rect drect = make_rect(tile.frame);
//        SDL_RenderCopy(engine->getRenderer(), tilesSheet->getTexture(), &srect, &drect);
//    }
}

GamePart::~GamePart()
{
    glDeleteBuffers(1, &vboVerts); // VAO references the buffer now
}
