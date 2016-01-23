//
//  DeleteNumberPowerUp.h
//  triptych
//
//  Created by Nick Enchev on 2014-10-07.
//
//

#ifndef __triptych__DeleteNumberPowerUp__
#define __triptych__DeleteNumberPowerUp__

#include <string>
#include "PowerUp.h"

using namespace std;

class DeleteNumberPowerUp : public PowerUp
{
public:
    DeleteNumberPowerUp();
    vector<GridPosition> run(Board &board, Player &player);
    bool canRecover(const Player &player) const;
};

#endif /* defined(__triptych__DeleteNumberPowerUp__) */
