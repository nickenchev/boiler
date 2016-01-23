//
//  PromoteNumberPowerUp.cpp
//  triptych
//
//  Created by Nick Enchev on 2014-10-17.
//
//

#include "PromoteNumberPowerUp.h"

PromoteNumberPowerUp::PromoteNumberPowerUp() : PowerUp()
{
    tier = 1;
    cost = 2;
    code = "PMT";
    name = string("Promote");
    help = string("Increases a number");
    
    selectionRules.push_back(SelectionRule::create(ValueValidation::REQUIRE_NON_EMPTY));
}

bool PromoteNumberPowerUp::canRecover(const Player &player) const
{
    return true;
}

vector<GridPosition> PromoteNumberPowerUp::run(Board &board, Player &player)
{
    GridPosition selection1 = board.getSelections()[0];
    BoardCell &cell1 = board.getCell(selection1);
    cell1.increaseValue();
    
    vector<GridPosition> changed;
    changed.push_back(selection1);

    return changed;
}
