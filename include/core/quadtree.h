#ifndef QUADTREE_H
#define QUADTREE_H

#include <memory>
#include <vector>
#include "rect.h"
#include "core/entity.h"

namespace Boiler
{

#define NUM_SUBNODES 4

class Quadtree
{
    const unsigned int maxObjects;
    const unsigned int maxSubLevels;

    int level;
	std::vector<std::pair<Boiler::Entity, Boiler::Rect>> objects;
    Rect bounds;
    Quadtree *nodes[NUM_SUBNODES];

    int getIndex(const Rect &rect) const;

public:
    Quadtree(int level, const Rect &bounds);
    ~Quadtree();

    void clear();
    void split();
	void insert(const Boiler::Entity &entity, Boiler::Rect frame);
    void retrieve(std::vector<std::shared_ptr<Entity>> &objects, const Rect &rect) const;
	void retrieve(std::vector<std::pair<Entity, Rect>> &objects, const Rect &rect) const;
};

}

#endif /* QUADTREE_H */
