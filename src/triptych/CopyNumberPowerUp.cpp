//
//  DuplicatePowerUp.cpp
//  triptych
//
//  Created by Nick Enchev on 2014-10-09.
//
//

#include "CopyNumberPowerUp.h"

CopyNumberPowerUp::CopyNumberPowerUp() : PowerUp()
{
    tier = 1;
    cost = 3;
    code = string("CPY");
    name = string("Copy");
    help = string("Make a copy of\na number");
    
    selectionRules.push_back(SelectionRule::create(ValueValidation::REQUIRE_NON_EMPTY));
    selectionRules.push_back(SelectionRule::create(ValueValidation::REQUIRE_EMPTY));
}

bool CopyNumberPowerUp::canRecover(const Player &player) const
{
    return false;
}

vector<GridPosition> CopyNumberPowerUp::run(Board &board, Player &player)
{
    GridPosition selection1 = board.getSelections()[0];
    GridPosition selection2 = board.getSelections()[1];
    BoardCell &cell1 = board.getCell(selection1);
    BoardCell &cell2 = board.getCell(selection2);

    //copy the state of cell1 into the empty cell2
    cell2 = cell1;
    
    vector<GridPosition> changed;
    changed.push_back(selection2);
    
    return changed;
}
