//
//  CopyNumberPowerUp.h
//  triptych
//
//  Created by Nick Enchev on 2014-10-09.
//
//

#ifndef __triptych__CopyNumberPowerUp__
#define __triptych__CopyNumberPowerUp__

#include "PowerUp.h"

class CopyNumberPowerUp : public PowerUp
{
public:
    CopyNumberPowerUp();

    vector<GridPosition> run(Board &board, Player &player) override;
    bool canRecover(const Player &player) const override;
};

#endif /* defined(__triptych__CopyNumberPowerUp__) */
