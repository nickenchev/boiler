//
//  BombPowerUp.h
//  triptych
//
//  Created by Nick Enchev on 2014-10-07.
//
//

#ifndef __triptych__BombPowerUp__
#define __triptych__BombPowerUp__

#include "PowerUp.h"

class BombPowerUp : public PowerUp
{
public:
    BombPowerUp();
    
    vector<GridPosition> run(Board &board, Player &player) override;
    bool canRecover(const Player &player) const override;
};

#endif /* defined(__triptych__BombPowerUp__) */
