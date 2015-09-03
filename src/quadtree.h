#ifndef QUADTREE_H
#define QUADTREE_H

#include <memory>
#include <vector>
#include "rect.h"

#define NUM_SUBNODES 4

class Entity;

class Quadtree
{
    const int maxObjects;
    const int maxSubLevels;

    int level;
    std::vector<std::shared_ptr<Entity>> objects;
    Rect bounds;
    Quadtree *nodes[NUM_SUBNODES];

    int getIndex(const Rect &rect) const;

public:
    Quadtree(int level, const Rect &bounds);
    ~Quadtree();

    void clear();
    void split();
    void insert(std::shared_ptr<Entity> entity);
    void retrieve(std::vector<std::shared_ptr<Entity>> &objects, const Rect &rect) const;
};

#endif /* QUADTREE_H */
