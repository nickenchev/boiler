#ifndef TILE_H
#define TILE_H

#include "entity.h"
#include "rect.h"

namespace Boiler { namespace tmx {

class TmxTile;

class Tile : public Entity
{
    const TmxTile &tmxTile;
public:
    Tile(const TmxTile &tmxTile, Rect frame) : Entity(frame), tmxTile(tmxTile) { }

    const TmxTile &getTmxTile() const { return tmxTile; }
};

} }
#endif /* TILE_H */
