#include "core/quadtree.h"
#include "core/entity.h"

#define MAX_OBJECTS 10
#define MAX_SUB_LEVELS 5 

using namespace Boiler;

Quadtree::Quadtree(int level, const Rect &bounds) : maxObjects(MAX_OBJECTS), maxSubLevels(MAX_SUB_LEVELS), bounds(bounds)
{
    //std::cout << "(" << bounds.position.x << "," << bounds.position.y << " " << bounds.size.width << "x" << bounds.size.height << ")" << std::endl;
    this->level = level;

    for (int i = 0; i < NUM_SUBNODES; ++i)
    {
        nodes[i] = nullptr;
    }
}

void Quadtree::clear()
{
    objects.clear();

    for (int i = 0; i < NUM_SUBNODES; ++i)
    {
        if (nodes[i] != nullptr)
        {
            nodes[i]->clear();
            nodes[i] = nullptr;
        }
    }
}

void Quadtree::split()
{
    int subWidth = bounds.size.width / 2;
    int subHeight = bounds.size.height / 2;
    int x = bounds.position.x;
    int y = bounds.position.y;

    const int newLevel = level + 1;
    nodes[0] = new Quadtree(newLevel, Rect(x + subWidth, y, subWidth, subHeight));
    nodes[1] = new Quadtree(newLevel, Rect(x, y, subWidth, subHeight));
    nodes[2] = new Quadtree(newLevel, Rect(x, y + subHeight, subWidth, subHeight));
    nodes[3] = new Quadtree(newLevel, Rect(x + subWidth, y + subHeight, subWidth, subHeight));
}

int Quadtree::getIndex(const Rect &rect) const
{
    int index = -1;
    double vMidpoint = bounds.position.x + bounds.size.width / 2;
    double hMidpoint = bounds.position.y + bounds.size.height / 2;

    bool topQuadrant = (rect.position.y < hMidpoint && rect.position.y + rect.size.height < hMidpoint);
    bool bottomQuadrant = (rect.position.y > hMidpoint);

    if (rect.position.x < vMidpoint && rect.position.x + rect.size.width < vMidpoint)
    {
        // object can fit in the left quadrant
        if (topQuadrant)
        {
            index = 1;
        }
        else if (bottomQuadrant)
        {
            index = 2;
        }
    }
    else if (rect.position.x > vMidpoint)
    {
        // object can fit in the right quadrant
        if (topQuadrant)
        {
            index = 0;
        }
        else if (bottomQuadrant)
        {
            index = 3;
        }
    }

    return index;
}

void Quadtree::insert(const Entity &entity, Rect frame)
{
    if (nodes[0] != nullptr)
    {
        int index = getIndex(frame);

        if (index != -1)
        {
            nodes[index]->insert(entity, frame);
            return;
        }
    }

	objects.push_back(std::make_pair(entity, frame));
    if (objects.size() > maxObjects && level < maxSubLevels)
    {
        if (nodes[0] == nullptr)
        {
            split();
        }
        int i = 0;
        while (i < objects.size())
        {
			int index = getIndex(frame);
            if (index != -1)
            {
                nodes[index]->insert(objects[i].first, objects[i].second);
                objects.erase(objects.begin() + i);
            }
            else
            {
                ++i;
            }
        }
    }
}

void Quadtree::retrieve(std::vector<std::pair<Entity, Rect>> &objects, const Rect &rect) const
{
    int index = getIndex(rect);
    if (index != -1 && nodes[0] != nullptr)
    {
        nodes[index]->retrieve(objects, rect);
    }
    else if (index == -1 && nodes[0] != nullptr)
    {
        for (int i = 0; i < NUM_SUBNODES; ++i)
        {
            nodes[i]->retrieve(objects, rect);
        }
    }
    objects.insert(objects.end(), this->objects.begin(), this->objects.end());
}

Quadtree::~Quadtree()
{
    for (int i = 0; i < NUM_SUBNODES; ++i)
    {
        if (nodes[i] != nullptr)
        {
            delete nodes[i];
            nodes[i] = nullptr;
        }
    }
}
