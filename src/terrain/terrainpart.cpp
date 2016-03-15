#include <iostream>
#include <boiler.h>
#include <array2d.h>
#include "terrainpart.h"
#include "pancamera.h"

const int resolution = 255;
const int terrainSize = 33;
const int roughness = 4;
Array2D<int> heightMap(terrainSize, terrainSize);

int getNumber()
{
    return rand() % resolution;
}

void diamonSquare(const int size)
{

}

TerrainPart::TerrainPart() : keys{0}
{
    // initialize the four corners
    heightMap.set(0, 0, getNumber()); // top-left
    heightMap.set(terrainSize- 1, 0, getNumber()); //top-right
    heightMap.set(terrainSize - 1, terrainSize- 1, getNumber()); //top-right
    heightMap.set(0, terrainSize - 1, getNumber()); //top-right

    // diamond-square
    diamonSquare(terrainSize);
}

void TerrainPart::onCreate()
{
    terrainSheet = Engine::getInstance().getSpriteLoader().loadSheet("data/terrain.json");
    Engine::getInstance().addKeyListener(this);

    // draw terrain
    const int tileSize = 64;
    int tileX = 0;
    int tileY = 0;
    for (int x = 0; x < terrainSize * tileSize; x += tileSize)
    {
        for (int y = 0; y < terrainSize * tileSize; y += tileSize)
        {
            auto tile = std::make_shared<Entity>(Rect(x, y, tileSize, tileSize));

            int height = heightMap.get(tileX, tileY);
            if (height < 30) // deep water
            {
                tile->spriteFrame = terrainSheet->getFrame("water.png");
            }
            else if (height < 50) // shallow water
            {
                tile->spriteFrame = terrainSheet->getFrame("shallow_water.png");
            }
            else if (height < 70) // regular land
            {
                tile->spriteFrame = terrainSheet->getFrame("land.png");
            }
            else // mountains
            {
                tile->spriteFrame = terrainSheet->getFrame("mountains.png");
            }

            addChild(tile);
            tileY++;
        }
        tileX++;
        tileY = 0;
    }

    Engine &engine = Engine::getInstance();

    // setup the camera
    Size screenSize = engine.getScreenSize();
    int mapWidth = terrainSize * tileSize;
    int mapHeight = terrainSize * tileSize;
    int camWidth = screenSize.getWidth() / engine.getRenderer().getGlobalScale().x;
    int camHeight = screenSize.getHeight() / engine.getRenderer().getGlobalScale().y;
    camera = std::make_shared<PanCemera>(Rect(0, 0, camWidth, camHeight), Size(mapWidth, mapHeight));
    engine.getRenderer().setCamera(camera);
}

void TerrainPart::update()
{
    const float speed = 10.0f;
    glm::vec3 cameraMove;
    
    if (keys[SDLK_w])
    {
        cameraMove = glm::vec3(0, -speed, 0.0f);
    }
    else if (keys[SDLK_s])
    {
        cameraMove = glm::vec3(0, speed, 0.0f);
    }

    if (keys[SDLK_a])
    {
        cameraMove = glm::vec3(-speed, 0, 0.0f);
    }
    else if (keys[SDLK_d])
    {
        cameraMove = glm::vec3(speed, 0, 0.0f);
    }

    camera->frame.position += cameraMove;
}

void TerrainPart::onKeyStateChanged(const KeyInputEvent &event)
{
    if (event.keyCode == SDLK_ESCAPE)
    {
        Engine::getInstance().quit();
    }
    else if (event.keyCode == SDLK_w)
    {
        keys[SDLK_w] = event.state == ButtonState::DOWN ? true : false;
    }
    else if (event.keyCode == SDLK_s)
    {
        keys[SDLK_s] = event.state == ButtonState::DOWN ? true : false;
    }
    else if (event.keyCode == SDLK_a)
    {
        keys[SDLK_a] = event.state == ButtonState::DOWN ? true : false;
    }
    else if (event.keyCode == SDLK_d)
    {
        keys[SDLK_d] = event.state == ButtonState::DOWN ? true : false;
    }
}
