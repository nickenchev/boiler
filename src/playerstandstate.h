#ifndef PLAYERSTANDSTATE_H
#define PLAYERSTANDSTATE_H

#include "entitystate.h"

class PlayerStandState : public EntityState
{
public:
    PlayerStandState(SpriteAnimation *animation) : EntityState(animation) { }

    void onEnter()
    {
    }
    void onExit()
    {
    }
    void handleInput()
    {

    }
};


#endif /* PLAYERSTANDSTATE_H */
