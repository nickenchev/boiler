#ifndef TILE_H
#define TILE_H

#include "entity.h"

namespace ensoft
{
    class TmxTile;
};

class Tile : public Entity
{
    const ensoft::TmxTile &tmxTile;
public:
    Tile(const ensoft::TmxTile &tmxTile) : tmxTile(tmxTile) { }

    const ensoft::TmxTile &getTmxTile() const { return tmxTile; }
};


#endif /* TILE_H */
