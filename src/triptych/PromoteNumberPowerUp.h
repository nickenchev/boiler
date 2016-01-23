//
//  PromoteNumberPowerUp.h
//  triptych
//
//  Created by Nick Enchev on 2014-10-17.
//
//

#ifndef __triptych__PromoteNumberPowerUp__
#define __triptych__PromoteNumberPowerUp__

#include "PowerUp.h"

class PromoteNumberPowerUp : public PowerUp
{
public:
    PromoteNumberPowerUp();
    
    vector<GridPosition> run(Board &board, Player &player);
    bool canRecover(const Player &player) const;
};

#endif /* defined(__triptych__PromoteNumberPowerUp__) */
