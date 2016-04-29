#include <iostream>
#include <random>
#include <boiler.h>
#include <array2d.h>
#include "terrainpart.h"
#include "pancamera.h"

struct Pixel
{
    Pixel() { }
    Pixel(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

const int mapWidth = 1024;
const int mapHeight = 1024;

const int terrainSize = 513;
const int resolution = 1024;
const int smallRandRange = 512;

Pixel pixelData[terrainSize * terrainSize];
Array2D<int> heightMap(terrainSize, terrainSize);

constexpr float getNormDenominator() { return resolution + smallRandRange; }
constexpr float getNormalized(int height) { return height / getNormDenominator(); }

std::mt19937 randEngine{std::random_device{}()};
std::uniform_int_distribution<int> heightRand(1, resolution);

int getNumber()
{
    int number = heightRand(randEngine);
    return number;
}

void diamondSquare(int size, int randRange)
{
    const float randMult = 0.8f;
    const int half = size / 2;
    const int whole = half * 2;

    std::uniform_int_distribution<int> smallRand(1, randRange);

    for (int y = 0; y < terrainSize - 1; y += size)
    {
        for (int x = 0; x < terrainSize - 1; x += size)
        {
            int tl = heightMap.get(x, y);
            int tr = heightMap.get(x + whole, y);
            int bl = heightMap.get(x, y + whole);
            int br = heightMap.get(x + whole, y + whole);
            int center = ((tl + tr + bl + br) / 4) + smallRand(randEngine);

            heightMap.set(x + half, y + half, center);

            int dn = (y - half >= 0) ? heightMap.get(x + half, y - half) : 0;
            int ds = (y + half <= terrainSize - 1) ? heightMap.get(x + half, y + half) : 0;
            int dw = (x - half >= 0) ? heightMap.get(x, y + half) : 0;
            int de = (x + half <= terrainSize - 1) ? heightMap.get(x + half, y + half) : 0;

            int n = ((tl + tr + dn + center) / 4) + smallRand(randEngine);
            int e = ((tr + br + de + center) / 4) + smallRand(randEngine);
            int s = ((br + bl + ds + center) / 4) + smallRand(randEngine);
            int w = ((tl + bl + dw + center) / 4) + smallRand(randEngine);
            heightMap.set(x + half, y, n);
            heightMap.set(x + whole, y + half, e);
            heightMap.set(x + half, y + whole, s);
            heightMap.set(x, y + half, w);
        }
    }

    if (size / 2 > 1)
    {
        diamondSquare(size / 2, randRange * randMult);
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
    std::cout << "Generating height-map" << std::endl;
	diamondSquare(terrainSize, smallRandRange);

    std::cout << "Generating texture data" << std::endl;
    glm::vec4 colour;
    for (int y = 0; y < terrainSize; ++y)
    {
        for (int x = 0; x < terrainSize; ++x)
        {
            const int height = heightMap.get(x, y);
            float normalized = getNormalized(height);
            if (normalized > 1.0f) normalized = 1.0f;

            int index = y * terrainSize + x;
            Pixel color;
            if (normalized < 0.5f) // deep water
            {
                color = Pixel(65, 76, 191, 255);
            }
            else if (normalized < 0.55f) // shallow water
            {
                color = Pixel(112, 120, 212, 255);
            }
            else if (normalized < 0.6f) // regular land
            {
                color = Pixel(29, 138, 80, 255);
            }
            else if (normalized < 0.65f) // forest
            {
                color = Pixel(10, 82, 43, 255);
            }
            else // mountains
            {
                color = Pixel(140, 140, 140, 255);
            }

            color = Pixel(255 * normalized, 255 * normalized, 255 * normalized, 255);

            pixelData[index] = color;
        }
    }
}

void TerrainPart::onCreate()
{
    terrainSheet = Engine::getInstance().getSpriteLoader().loadSheet("data/terrain.json");
    Engine::getInstance().addKeyListener(this);

	auto terrainTexture = Engine::getInstance().getRenderer().createTexture("", Size(terrainSize, terrainSize), pixelData);
    auto map = std::make_shared<Entity>(Rect(0, 0, mapWidth, mapHeight));

    procSheet = Engine::getInstance().getSpriteLoader().loadSheet(terrainTexture);
    map->spriteFrame = procSheet->getFirstFrame();
    addChild(map);
    // draw terrain
    //const int tileSize = 4;
    /*
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
            if (normalized < 0.5f) // deep water
            {
                frame = terrainSheet->getFrame("water.png");
            }
            else if (normalized < 0.55f) // shallow water
            {
                frame = terrainSheet->getFrame("shallow_water.png");
            }
            else if (normalized < 0.6f) // regular land
            {
                frame = terrainSheet->getFrame("land.png");
            }
            else if (normalized < 0.65f) // forest
            {
                frame = terrainSheet->getFrame("forest.png");
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
    */

    Engine &engine = Engine::getInstance();

    // setup the camera
    Size screenSize = engine.getScreenSize();
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

    //camera->frame.position += cameraMove;
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
