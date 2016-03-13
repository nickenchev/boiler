#include <iostream>
#include <boiler.h>
#include <array2d.h>
#include "terrainpart.h"

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
    const int half = size / 2;
    const int scale = roughness * size;

    if (half >= 1)
    {
        // square pass
        for (int y = half; y < terrainSize; y += size)
        {
            for (int x = half; x < terrainSize; x += size)
            {
                const int a = heightMap.sample(x - half, y - half); // top-left
                const int b = heightMap.sample(x + half, y - half); // top-right
                const int c = heightMap.sample(x + half, y + half); // bottom-right
                const int d = heightMap.sample(x - half, y + half); // bottom-left
                const int avg = (a + b + c + d) / 4;
                heightMap.set(x, y, avg);
            }
        }

        // diamond pass
        for (int y = 0; y < terrainSize; y += half)
        {
            for (int x = (y + half) % size; x <= size; x += size)
            {
                const int a = heightMap.sample(x, y - half); // top
                const int b = heightMap.sample(x + half, y); // right
                const int c = heightMap.sample(x, y + half); // bottom
                const int d = heightMap.sample(x - half, y); // left
                const int avg = (a + b + c + d) / 4;
                heightMap.set(x, y, avg);
            }
        }

        diamonSquare(half);
    }
}

TerrainPart::TerrainPart()
{

    // initialize the four corners
    heightMap.set(0, 0, getNumber()); // top-left
    heightMap.set(terrainSize- 1, 0, getNumber()); //top-right
    heightMap.set(terrainSize - 1, terrainSize- 1, getNumber()); //top-right
    heightMap.set(0, terrainSize - 1, getNumber()); //top-right

    // diamond-square
    diamonSquare(terrainSize);

    for (int x = 0; x < terrainSize; ++x)
    {
        for (int y = 0; y < terrainSize; ++y)
        {
            int height = heightMap.get(x, y);
            if (height < 30) // deep water
            {
                std::cout << "~";
            }
            else if (height < 60) // shallow water
            {
                std::cout << "#";
            }
            else if (height < 100) // regular land
            {
                std::cout << "X";
            }
            else if (height < 150) // hills
            {
                std::cout << "7";
            }
            else // mountains
            {
                std::cout << "^";
            }
        }
        std::cout << std::endl;
    }

}

void TerrainPart::onCreate()
{
    Engine::getInstance().addKeyListener(this);
}

void TerrainPart::onKeyStateChanged(const KeyInputEvent &event)
{
    if (event.keyCode == SDLK_ESCAPE)
    {
        Engine::getInstance().quit();
    }
}
