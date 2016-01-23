//
//  Persistence.cpp
//  triptych
//
//  Created by Nick Enchev on 2015-06-25.
//
//

#include "Persistence.h"
#include "cocos2d.h"

USING_NS_CC;

bool Persistence::isFlagSet(std::string flag)
{
    return UserDefault::getInstance()->getBoolForKey(flag.c_str());
}

void Persistence::setFlag(std::string flag, bool value)
{
    UserDefault::getInstance()->setBoolForKey(flag.c_str(), value);
}

void Persistence::resetFlag(std::string flag)
{
    setFlag(flag, false);
}