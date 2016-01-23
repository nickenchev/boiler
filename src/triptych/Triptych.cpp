//
//  Triptych.cpp
//  triptych
//
//  Created by Nick Enchev on 2015-01-02.
//
//

#include "Triptych.h"

Triptych::Triptych(TriptychDirection direction)
{
    this->direction = direction;
}

void Triptych::addPosition(GridPosition position)
{
    positions.push_back(position);
}