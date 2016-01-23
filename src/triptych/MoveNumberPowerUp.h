//
//  MoveNumberPowerUp.h
//  triptych
//
//  Created by Nick Enchev on 2014-10-09.
//
//

#ifndef __triptych__MoveNumberPowerUp__
#define __triptych__MoveNumberPowerUp__

#include "PowerUp.h"

class MoveNumberPowerUp : public PowerUp
{
public:
    MoveNumberPowerUp();
    vector<GridPosition> run(Board &board, Player &player);
    bool canRecover(const Player &player) const;
};

#endif /* defined(__triptych__MoveNumberPowerUp__) */
