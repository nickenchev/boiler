#include <iostream>
#include <random>
#include <functional>

#include "terrainpart.h"
#include "core/engine.h"

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

const bool grayscale = false;

const float water = 0.4f;
const float shallowWater = 0.45;
const float land = 0.65f;
const float mountains = 0.75f;

const int mapWidth = 1024 * 20;
const int mapHeight = 1024 * 20;

const int interpolationPasses = 2;
const bool shouldInterpolate = true;

const int terrainSize = 2049;
const int resolution = 256;
const int smallRandRange = 90;
const int smallRandDecrease = 10;

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

int getSmallNumber(int rangeEnd)
{
    if (rangeEnd < 1)
    {
        rangeEnd = 1;
    }
    std::uniform_int_distribution<int> smallRand(-rangeEnd, rangeEnd);
    return smallRand(randEngine);
}

int clampHeight(int height)
{
    int result = height;
    if (result < 0)
    {
        result = 0;
    }
    else if (result > resolution)
    {
        result = resolution;
    }

    return result;
}

void diamondSquare(int size, int randRange)
{
    const int half = size / 2;
    const int whole = half * 2;

    for (int y = 0; y < terrainSize - 1; y += size)
    {
        for (int x = 0; x < terrainSize - 1; x += size)
        {

            int tl = heightMap.get(x, y);
            int tr = heightMap.get(x + whole, y);
            int bl = heightMap.get(x, y + whole);
            int br = heightMap.get(x + whole, y + whole);
            int center = ((tl + tr + bl + br) / 4) + getSmallNumber(randRange);

            heightMap.set(x + half, y + half, clampHeight(center));

            int dn = (y - half >= 0) ? heightMap.get(x + half, y - half) : heightMap.get(x + half, terrainSize - half);
            int ds = (y + half <= terrainSize - 1) ? heightMap.get(x + half, y + half) : heightMap.get(x + half, half - 1);
            int dw = (x - half >= 0) ? heightMap.get(x, y + half) : heightMap.get(terrainSize - half, y + half);
            int de = (x + half <= terrainSize - 1) ? heightMap.get(x + half, y + half) : heightMap.get(half - 1, y + half);

            int n = ((tl + tr + dn + center) / 4) + getSmallNumber(randRange);
            int e = ((tr + br + de + center) / 4) + getSmallNumber(randRange);
            int s = ((br + bl + ds + center) / 4) + getSmallNumber(randRange);
            int w = ((tl + bl + dw + center) / 4) + getSmallNumber(randRange);

            heightMap.set(x + half, y, clampHeight(n));
            heightMap.set(x + whole, y + half, clampHeight(e));
            heightMap.set(x + half, y + whole, clampHeight(s));
            heightMap.set(x, y + half, clampHeight(w));
        }
    }

    if (size / 2 > 1)
    {
        diamondSquare(size / 2, randRange - smallRandDecrease);
    }
}

void interpolate()
{
    for (int p = 0; p < interpolationPasses; ++p)
    {
        for (int y = 0; y < terrainSize; ++y)
        {
            for (int x = 0; x < terrainSize; ++x)
            {
                if (x > 0 && x < terrainSize - 1 &&
                    y > 0 && y < terrainSize - 1)
                {
                    int val = (heightMap.get(x, y - 1) + heightMap.get(x, y + 1) + heightMap.get(x - 1, y) + heightMap.get(x + 1, y) + heightMap.get(x, y)) / 5;
                    heightMap.set(x, y, val);
                }
            }
        }
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

    if (shouldInterpolate)
    {
        interpolate();
    }

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
            if (normalized < water) // deep water
            {
                color = Pixel(65, 76, 191, 255);
            }
            else if (normalized < shallowWater) // shallow water
            {
                color = Pixel(80, 90, 212, 255);
            }
            else if (normalized < land) // regular land
            {
                color = Pixel(29, 138, 80, 255);
            }
            else if (normalized < mountains) // mountains
            {
                color = Pixel(100, 100, 100, 255);
            }
            else // mountain peaks
            {
                color = Pixel(140, 140, 140, 255);
            }

            if (grayscale)
            {
                color = Pixel(255 * normalized, 255 * normalized, 255 * normalized, 255);
            }

            pixelData[index] = color;
        }
    }

    terrainSheet = Engine::getInstance().getSpriteLoader().loadSheet("data/terrain.json");
	auto terrainTexture = Engine::getInstance().getRenderer().createTexture("", Size(terrainSize, terrainSize), pixelData);
    procSheet = Engine::getInstance().getSpriteLoader().loadSheet(terrainTexture);
}

void TerrainPart::onCreate()
{
    auto map = std::make_shared<Entity>(Rect(0, 0, mapWidth, mapHeight));
    map->spriteFrame = procSheet->getFirstFrame();
    addChild(map);

    auto miniMap = std::make_shared<Entity>(Rect(10, 10, 500, 500));
    miniMap->spriteFrame = procSheet->getFirstFrame();
    miniMap->absolute = true;
    addChild(miniMap);

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

    Engine::getInstance().addTouchMotionListener(std::bind(&TerrainPart::touchMotion, this, std::placeholders::_1));
    Engine::getInstance().addMouseMotionListener(std::bind(&TerrainPart::mouseMotion, this, std::placeholders::_1));
    Engine::getInstance().addKeyInputListener(std::bind(&TerrainPart::keyInput, this, std::placeholders::_1));
}

void TerrainPart::touchMotion(const TouchMotionEvent &event)
{
    const float panSpeed = 1800.0f;
    cameraMove = glm::vec3(-event.xDistance * panSpeed , -event.yDistance * panSpeed, 0.0f);
    this->camera->frame.position += cameraMove;
}

void TerrainPart::mouseMotion(const MouseMotionEvent &event)
{
    //const float panSpeed = 100;
    //cameraMove = glm::vec3(-event.xDistance * panSpeed , -event.yDistance * panSpeed, 0.0f);
    //this->camera->frame.position += cameraMove;
}

void TerrainPart::keyInput(const KeyInputEvent &event)
{
    const float speed = 10.0f;
	if (event.state == ButtonState::DOWN)
	{
		switch (event.keyCode)
		{
			case SDLK_w:
			{
				cameraMove = glm::vec3(0, -speed, 0.0f);
				break;
			}
			case SDLK_s:
			{
				cameraMove = glm::vec3(0, speed, 0.0f);
				break;
			}
			case SDLK_a:
			{
				cameraMove = glm::vec3(-speed, 0, 0.0f);
				break;
			}
			case SDLK_d:
			{
				cameraMove = glm::vec3(speed, 0, 0.0f);
				break;
			}
		}
	}
	else if (event.state == ButtonState::UP)
	{
		cameraMove = glm::vec3(0, 0 ,0);
		switch (event.keyCode)
		{
			case SDLK_ESCAPE:
			{
				Engine::getInstance().quit();
			}
		}
    }
	this->camera->frame.position += cameraMove;
}

void TerrainPart::update()
{
	this->camera->frame.position += cameraMove;
}
