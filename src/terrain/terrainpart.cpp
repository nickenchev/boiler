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
    const int max = size - 1;
    const int half = max / 2;
    for (int y = 0; y < terrainSize; y += max)
    {
        for (int x = 0; x < terrainSize; x += max)
        {
            // diamond step
            const int maxX = x + max;
            const int maxY = y + max;
            int a = heightMap.get(x, y);
            int b = heightMap.get(maxX, y);
            int c = heightMap.get(maxX, maxY);
            int d = heightMap.get(x, maxY);

            int avg = (a + b + c + d) / 4;
            heightMap.set(half, half, avg);

            // square step

            int n = ((a + b) / 2);
            int e = ((b + c) / 2);
            int s = ((c + d) / 2);
            int w = ((d + a) / 2);
            heightMap.set(half, y, n);
            heightMap.set(maxX, half, e);
            heightMap.set(half, maxY, s);
            heightMap.set(x, half, w);
        }
    }
    if (half > 1)
    {
        diamonSquare(half);
    }
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
    const int tileSize = 16;
    int tileX = 0;
    int tileY = 0;
    for (int x = 0; x < terrainSize; ++x)
    {
        for (int y = 0; y < terrainSize; ++y)
        {
            auto tile = std::make_shared<Entity>(Rect(tileX, tileY, tileSize, tileSize));
            const int height = heightMap.get(x, y);

            float normalized = height / 255.0f;
            if (normalized > 1.0f) normalized = 1.0f;
            tile->color = glm::vec4(normalized, normalized, normalized, 1.0f);

            if (height < 30) // deep water
            {
            tile->spriteFrame = terrainSheet->getFrame("water.png");
            }
            else if (height < 50) // shallow water
            {
            tile->spriteFrame = terrainSheet->getFrame("shallow_water.png");
            }
            else if (height < 120) // regular land
            {
            tile->spriteFrame = terrainSheet->getFrame("land.png");
            }
            else // mountains
            {
            tile->spriteFrame = terrainSheet->getFrame("mountains.png");
            }

            addChild(tile);
            tileY += tileSize;
        }
        tileX += tileSize;
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
