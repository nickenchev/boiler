#include <iostream>
#include <random>
#include <boiler.h>
#include <array2d.h>
#include "terrainpart.h"
#include "pancamera.h"

const int resolution = 255;
const int smallRes = 10;
constexpr float getNormDenominator() { return resolution + smallRes; }

const int terrainSize = 129;
const int roughness = 4;

Array2D<int> heightMap(terrainSize, terrainSize);
std::mt19937 randEngine{std::random_device{}()};
std::uniform_int_distribution<int> heightRand(1, resolution);

int getNumber()
{
    int number = heightRand(randEngine);
    return number;
}

void diamonSquare(const int size)
{
    const int half = size / 2;
    const int max = half * 2;
    
    for (int y = 0; y < terrainSize - 1; y += size)
    {
        for (int x = 0; x < terrainSize - 1; x += size)
        {
            std::uniform_int_distribution<int> smallRand(1, smallRes);
            const int midX = x + half;
            const int midY = y + half;
            const int maxX = x + max;
            const int maxY = y + max;

            // diamond step
            const int nw = heightMap.get(x, y);
            const int ne = heightMap.get(maxX, y);
            const int se = heightMap.get(maxX, maxY);
            const int sw = heightMap.get(x, maxY);
            const int avg = ((nw + ne + se + sw) / 4) + smallRand(randEngine);
            heightMap.set(midX, midY, avg);

            // square step
            const int n = ((nw + ne) / 2) + smallRand(randEngine);
            const int e = ((ne + se) / 2) + smallRand(randEngine);
            const int s = ((se + sw) / 2) + smallRand(randEngine);
            const int w = ((sw + nw) / 2) + smallRand(randEngine);
            heightMap.set(midX, 0, n);
            heightMap.set(maxX, midY, e);
            heightMap.set(midX, maxY, s);
            heightMap.set(0, midY, w);
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
    const int tileSize = 8;
    int tileX = 0;
    int tileY = 0;
    for (int x = 0; x < terrainSize; ++x)
    {
        for (int y = 0; y < terrainSize; ++y)
        {
            auto tile = std::make_shared<Entity>(Rect(tileX, tileY, tileSize, tileSize));
            const int height = heightMap.get(x, y);

            float normalized = height / getNormDenominator();
            if (normalized > 1.0f) normalized = 1.0f;
            tile->color = glm::vec4(normalized, normalized, normalized, 1.0f);

            const SpriteSheetFrame *frame = nullptr;
            if (normalized < 0.6f) // deep water
            {
                frame = terrainSheet->getFrame("water.png");
            }
            else if (normalized < 0.65f) // shallow water
            {
                frame = terrainSheet->getFrame("shallow_water.png");
            }
            else if (normalized < 0.7f) // regular land
            {
                frame = terrainSheet->getFrame("land.png");
            }
            else // mountains
            {
                frame = terrainSheet->getFrame("mountains.png");
            }
            tile->spriteFrame = frame;

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
