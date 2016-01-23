//
//  MessageSpriteInfo.h
//  triptych
//
//  Created by Nick Enchev on 2015-03-05.
//
//

#ifndef triptych_MessageSpriteInfo_h
#define triptych_MessageSpriteInfo_h

#include <string>
#include "Pos2D.h"

class MessageSpriteInfo
{
public:
    std::string spriteName;
    Pos2D position;
    int rotation;
    
    MessageSpriteInfo() : rotation(0) { }
    MessageSpriteInfo(std::string spriteName, Pos2D position, int rotation) : spriteName(spriteName), position(position)
    {
        this->rotation = rotation;
    }
};

#endif
