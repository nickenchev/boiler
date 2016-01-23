//
//  BankFromBoardPowerUp.h
//  triptych
//
//  Created by Nick Enchev on 2015-01-16.
//
//

#ifndef __triptych__BankFromBoardPowerUp__
#define __triptych__BankFromBoardPowerUp__

#include "PowerUp.h"

class BankFromBoardPowerUp : public PowerUp
{
public:
    BankFromBoardPowerUp();

    vector<GridPosition> run(Board &board, Player &player);
    bool canRecover(const Player &player) const;
};

#endif /* defined(__triptych__BankFromBoardPowerUp__) */
