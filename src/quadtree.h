#ifndef QUADTREE_H
#define QUADTREE_H

#include <vector>
#include "rect.h"

#define MAX_OBJECTS 10
#define MAX_SUB_LEVELS 10
#define NUM_SUBNODES 4

class Entity;

class Quadtree
{
    const int maxObjects;
    const int maxSubLevels;

    int level;
    std::vector<Entity *> objects;
    Rect bounds;
    Quadtree *nodes[NUM_SUBNODES];

    int getIndex(const Rect &rect) const;

public:
    Quadtree(int level, const Rect &bounds);
    ~Quadtree();

    void clear();
    void split();
    void insert(Entity *entity);
    void retrieve(std::vector<Entity *> &objects, const Rect &rect) const;
};

#endif /* QUADTREE_H */
