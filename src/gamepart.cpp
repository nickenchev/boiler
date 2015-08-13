#include <iostream>
#include <SDL.h>

#include "engine.h"
#include "gamepart.h"
#include "spritesheet.h"
#include "spritesheetframe.h"

#include "sdl_util.h" //TODO: Meh?

// LOAD TMX Stuff
#include <glm/glm.hpp>
#include <map>
#include <zlib.h>
#include "tinyxml2.h"
#include "base64.h"
#include "string_util.h"

// OpenGL Rendering
#include "opengl.h"
#define GLM_COMPILER 0
#include <glm/gtc/matrix_transform.hpp>

using namespace tinyxml2;
using namespace std;

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

GamePart::GamePart(Engine *engine) : Part(engine), qtree(0, Rect(0, 0, engine->getScreenWidth(), engine->getScreenHeight())),
                                     gravity(9.8f)
{
    //do some loading
    playerSheet = engine->getSpriteLoader().loadSheet("data/zodas2.json");
    tilesSheet = engine->getSpriteLoader().loadSheet("data/tiles.json");

    //basic player setup
    player = addEntity(std::make_unique<Entity>(Rect(30, 550, 15, 31)));
    player->spriteSheet = playerSheet;

    // animation setup
    frameNum = 1;
    numFrames = 2;
    animTime = 0;
    timePerFrame = 0.15f;

    walkLeft.push_back(playerSheet->getFrame("walk_left_01.png"));
    walkLeft.push_back(playerSheet->getFrame("walk_left_02.png"));
    walkRight.push_back(playerSheet->getFrame("walk_right_01.png"));
    walkRight.push_back(playerSheet->getFrame("walk_right_02.png"));

    currentAnimation = &walkRight;

    // physics setup
    yLimit = 0;
    jumping = false;

    // tilemap setup
    tmxMap = std::move(loadTmx());
    Entity tileMap[100][100];

    // create the entities from the tile info
    int tileNum = 0;
    int x = 0, y = 0;
    for (int r = 0; r < tmxMap->width; ++r)
    {
        for (int c = 0; c < tmxMap->height; ++c)
        {
            int tileGid = tmxMap->layers[0]->tiles[tileNum];
            if (tileGid)
            {
                const ensoft::TmxTile *tmxTile= tmxMap->allTiles[tileGid];

                Entity *tile = addEntity(std::make_unique<Entity>(Rect(x, y, tmxMap->tilewidth, tmxMap->tileheight)));
                const SpriteSheetFrame *tileFrame = tilesSheet->getFrame(tmxTile->image.source);
                tile->pivot = glm::vec2(0, 0);
                tile->spriteSheet = tilesSheet;
                tile->spriteFrame = tileFrame;
            }

            x += tmxMap->tilewidth;
            tileNum++;
        }
        y += tmxMap->tileheight;
        x = 0;
    }
    // setup the shader
    program = std::make_unique<ShaderProgram>("shader");
    mvpUniform = glGetUniformLocation(program->getShaderProgram(), "MVP");
}

void GamePart::start()
{
}

void GamePart::handleInput()
{
    // check keyboard and modify state
    if (engine->keyState(SDLK_a))
    {
        currentAnimation = &walkLeft;
        velocity.x = -50;
    }
    else if (engine->keyState(SDLK_d))
    {
        currentAnimation = &walkRight;
        velocity.x = 50;
    }
    else
    {
        velocity.x = 0;
    }
    if (engine->keyState(SDLK_ESCAPE))
    {
        engine->quit();
    }
    // only change the velocity if there's a jump
    if (engine->keyState(SDLK_j))
    {
        if (velocity.y == 0)
        {
            velocity.y = -200;
            jumping = true;
        }
    }
}

bool clipLine(int d, const Rect &box, const glm::vec2 &v0, const glm::vec2 &v1, float &fLow, float &fHigh)
{
    bool isIntersection = true;

    // get the fraction of the line (v0->v1) that intersects the box
    float boxDimLow = box.position[d];
    float boxDimHigh = box.position[d] + ((d == 0) ? box.size.width : box.size.height);
    float fDimLow = (boxDimLow - v0[d]) / (v1[d] - v0[d]);
    float fDimHigh = (boxDimHigh - v0[d]) / (v1[d] - v0[d]);

    // swap if needed, varies depending on direction of ray
    if (fDimLow > fDimHigh)
    {
        std::swap(fDimLow, fDimHigh);
    }

    // make sure we're not completely missing
    if (fDimHigh < fLow)
    {
        isIntersection = false;
    }
    else if (fDimLow > fHigh)
    {
        isIntersection = false;
    }
    else
    {
        fLow = std::max(fDimLow, fLow);
        fHigh = std::min(fDimHigh, fHigh);
    }

    if (fLow > fHigh)
    {
        isIntersection = false;
    }

    return isIntersection;
}

bool rayCollides(const glm::vec2 &v0, const glm::vec2 &v1, const Rect &aabb, glm::vec2 &vIntersect)
{
    // ray cast and see if the player collides with this
    float fLow = 0;
    float fHigh = 1;

    // clip x (0) and y(0) dimensions to get the line segment that collides
    bool isIntersection = false;
    if (clipLine(0, aabb, v0, v1, fLow, fHigh))
    {
        if (clipLine(1, aabb, v0, v1, fLow, fHigh))
        {
            // get the full vector (b) and the vector the represents the first intersection point on the box
            glm::vec2 b = v1 - v0;
            vIntersect = v0 + b * fLow; // fLow is the fraction that represents the initial intersection

            isIntersection = true;
        }
    }
    return isIntersection;
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
    player->spriteFrame = (*currentAnimation)[frameNum - 1];

    // check what the player may collide with
    qtree.clear();
    for (auto &e : entities)
    {
        qtree.insert(e.get());
    }
    std::vector<Entity*> closeObjects;
    qtree.retrieve(closeObjects, player->frame);

    // check for collisions in the player's movement
    bool bottomCollide = false;
    for (auto e : closeObjects)
    {
        if (e != player)
        {
            float numRays = 3;

            // check bottom
            for (int r = 0; r < numRays; ++r)
            {
                float rayInterval = player->frame.size.width / (numRays - 1);
                float rayOffset = r * rayInterval;

                glm::vec2 v0(player->frame.position.x + rayOffset, player->frame.getMaxY());
                glm::vec2 v1 = v0 + velocity * delta;
                glm::vec2 vIntersect;
                if (rayCollides(v0, v1, e->frame, vIntersect))
                {
                    if (vIntersect.y < yLimit || yLimit == 0)
                    {
                        yLimit = vIntersect.y;
                    }
                    bottomCollide = true;
                    jumping = false;
                }
            }

            // check right
            if (velocity.x > 0)
            {
                for (int r = 0; r < numRays; ++r)
                {
                    float rayInterval = player->frame.size.width / (numRays - 1);
                    float rayOffset = r * rayInterval;

                    glm::vec2 v0(player->frame.getMaxX(), player->frame.position.y + rayOffset);
                    glm::vec2 v1 = v0 + velocity * delta;
                    glm::vec2 vIntersect;
                    if (rayCollides(v0, v1, e->frame, vIntersect))
                    {
                        velocity.x = 0;
                    }
                }
            }
            else if (velocity.x < 0)
            {
                for (int r = 0; r < numRays; ++r)
                {
                    float rayInterval = player->frame.size.width / (numRays - 1);
                    float rayOffset = r * rayInterval;

                    glm::vec2 v0(player->frame.getMinX(), player->frame.position.y + rayOffset);
                    glm::vec2 v1 = v0 + velocity * delta;
                    glm::vec2 vIntersect;
                    if (rayCollides(v0, v1, e->frame, vIntersect))
                    {
                        velocity.x = 0;
                    }
                }
            }
        }
    }

    if (!bottomCollide)
    {
        yLimit = 0;
    }
    // figure out whether to apply gravity
    bool grounded = (player->frame.getMaxY() <= yLimit);
    if (grounded && !jumping)
    {
        velocity.y = 0;
    }
    else
    {
        velocity.y += gravity;
    }

    player->frame.position += velocity * delta;
}

void GamePart::render()
{
    glUseProgram(program->getShaderProgram());

    for (auto &entity : entities)
    {
        drawEntity(entity.get());
    }

    glUseProgram(0);
}

void GamePart::drawEntity(Entity *entity)
{
    // set the vao for the current sprite
    glBindVertexArray(entity->getVao());

    // binds the current frames texture VBO and ensure it is linked to the current VAO
    glBindBuffer(GL_ARRAY_BUFFER, entity->spriteFrame->getTexCoordsVbo());
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);

    // opengl sprite render
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(getEngine()->getScreenWidth()),
                                      static_cast<GLfloat>(getEngine()->getScreenHeight()), 0.0f, -1.0f, 1.0f);

    const glm::mat4 &model = entity->getMatrix();
    projection = projection * model;

    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, &projection[0][0]);

    // the sprite becomes TEXTURE0 in the shader
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, entity->spriteSheet->getTexture());

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

GamePart::~GamePart()
{
}

Entity *GamePart::addEntity(std::unique_ptr<Entity> entity)
{
    entities.push_back(std::move(entity));
    return entities.back().get();
}
