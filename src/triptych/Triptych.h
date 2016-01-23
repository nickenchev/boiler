//
//  Triptych.h
//  triptych
//
//  Created by Nick Enchev on 2015-01-02.
//
//

#ifndef __triptych__Triptych__
#define __triptych__Triptych__

#include <vector>
#include "GridPosition.h"

using namespace std;

enum class TriptychDirection
{
    NONE,
    UP,
    DOWN,
    LEFT,
    RIGHT,
    DOWN_LEFT,
    DOWN_RIGHT,
    UP_LEFT,
    UP_RIGHT
};

class Triptych
{
    vector<GridPosition> positions;
    TriptychDirection direction;

public:
    Triptych(TriptychDirection direction);
    const GridPosition &operator[](int i) { return positions[i]; }
    const GridPosition &operator[](int i) const { return positions[i]; }
    
    TriptychDirection getDirection() const { return direction; }
    int size() const { return static_cast<int>(positions.size()); }
    void addPosition(GridPosition position);
};

#endif /* defined(__triptych__Triptych__) */
