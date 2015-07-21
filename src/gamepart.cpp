#include <iostream>
#include <SDL.h>

#include "engine.h"
#include "gamepart.h"
#include "spritesheet.h"
#include "spritesheetframe.h"

#include "tinyxml2.h"
#include "tmx.h"

using namespace tinyxml2;

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
        XMLElement *xmlmap = doc.FirstChildElement("map");
        Map map;
        map.version = xmlmap->Attribute("version");
        map.orientation = xmlmap->Attribute("orientation");
        map.width = xmlmap->IntAttribute("width");
        map.height = xmlmap->IntAttribute("height");
        map.tilewidth = xmlmap->IntAttribute("tilewidth");
        map.tileheight = xmlmap->IntAttribute("tileheight");
        map.renderorder = xmlmap->Attribute("renderorder");

        // load all the tilesets
        XMLElement *xtileset = xmlmap->FirstChildElement("tileset");
        while (xtileset)
        {
            TileSet tileSet;
            tileSet.firstgid = xtileset->IntAttribute("firstgid");

            const char *source = xtileset->Attribute("source");
            if (source)
            {
                tileSet.source = source;
            }
            tileSet.name = xtileset->Attribute("name");
            tileSet.tilewidth = xtileset->IntAttribute("tilewidth");
            tileSet.tileheight = xtileset->IntAttribute("tileheight");
            tileSet.spacing = xtileset->IntAttribute("spacing");
            tileSet.margin = xtileset->IntAttribute("margin");
            map.tilesets.push_back(tileSet);

            // try to find another tileset
            xtileset = xtileset->NextSiblingElement("tileset");
        }

        XMLElement *xlayer = xmlmap->FirstChildElement("layer");
        while (xlayer)
        {
            Layer layer;
            layer.name = xlayer->Attribute("name");
            layer.x = xlayer->IntAttribute("x");
            layer.y = xlayer->IntAttribute("y");
            layer.width = xlayer->IntAttribute("width");
            layer.height = xlayer->IntAttribute("height");
            layer.opacity = xlayer->FloatAttribute("opacity");
            layer.visible = xlayer->IntAttribute("visible");
            map.layers.push_back(layer);
            
            xlayer = xlayer->NextSiblingElement("layer");
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
