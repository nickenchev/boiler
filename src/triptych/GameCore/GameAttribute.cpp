//
//  GameAttribute.cpp
//  triptych
//
//  Created by Nick Enchev on 2015-02-17.
//
//

#include "GameAttribute.h"
#include <math.h>

GameAttribute::GameAttribute() : GameAttribute(0, 0, 0)
{
}
GameAttribute::GameAttribute(unsigned int initCount, unsigned long basePoints, float increaseMultiplier)
{
    itemCount = initCount;
    points = 0;
    this->basePoints = basePoints;
    this->increaseMultiplier = increaseMultiplier;
    gainRangeStart = 0;
    gainRangeEnd = basePoints;
    pointsForNextGain = basePoints;
}

int GameAttribute::addPoints(unsigned long points, const GameAttribute *modifier)
{
    this->points += points;
    int prevCount = itemCount;
    while (this->points >= gainRangeEnd)
    {
        //calculate the multiplier (based on modifier if passed in)
        int exponent = modifier ? modifier->getCount() : getCount();
        
        ++itemCount;
        gainRangeStart += pointsForNextGain;
        pointsForNextGain = pow(increaseMultiplier, exponent) * basePoints;
        gainRangeEnd += pointsForNextGain;
    }

    return itemCount - prevCount;
}

bool GameAttribute::subtract(int count)
{
    bool hadSufficient = count <= itemCount;
    if (hadSufficient)
    {
        itemCount -= count;
    }
    
    return hadSufficient;
}