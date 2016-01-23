//
//  GameAttribute.h
//  triptych
//
//  Created by Nick Enchev on 2015-02-17.
//
//

#ifndef __triptych__GameAttribute__
#define __triptych__GameAttribute__

#include <vector>

class GameAttribute
{
    unsigned int itemCount;
    unsigned long points;
    unsigned long basePoints;
    unsigned long gainRangeStart, gainRangeEnd;
    unsigned long pointsForNextGain;
    float increaseMultiplier;
    
public:
    GameAttribute();
    GameAttribute(unsigned int initCount, unsigned long basePoints, float increaseMultiplier);

    unsigned int getCount() const { return itemCount; }
    unsigned long getPoints() const { return points; }
    bool subtract(int count);
    
    float getCompletion() const { return (float)(points - gainRangeStart) / pointsForNextGain; }
    
    int addPoints(unsigned long points, const GameAttribute *modifier = nullptr);
};

#endif /* defined(__triptych__GameAttribute__) */
