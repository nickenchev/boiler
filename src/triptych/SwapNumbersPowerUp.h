//
//  SwapNumbersPowerUp.h
//  triptych
//
//  Created by Nick Enchev on 2014-10-02.
//
//

#ifndef __triptych__SwapNumbersPowerUp__
#define __triptych__SwapNumbersPowerUp__

#include "PowerUp.h"

class SwapNumbersPowerUp : public PowerUp
{
public:
    SwapNumbersPowerUp();

    vector<GridPosition> run(Board &board, Player &player) override;
    bool canRecover(const Player &player) const;
};

#endif /* defined(__triptych__SwapNumbersPowerUp__) */
