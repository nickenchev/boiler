//
//  SwapNumbersPowerUp.cpp
//  triptych
//
//  Created by Nick Enchev on 2014-10-02.
//
//

#include "SwapNumbersPowerUp.h"

SwapNumbersPowerUp::SwapNumbersPowerUp() : PowerUp()
{
    tier = 2;
    cost = 3;
    code = string("SWP");
    name = string("Swap");
    help = string("Swap 2 numbers");

    selectionRules.push_back(SelectionRule::create(ValueValidation::REQUIRE_NON_EMPTY));
    selectionRules.push_back(SelectionRule::create(ValueValidation::REQUIRE_NON_EMPTY));
}

bool SwapNumbersPowerUp::canRecover(const Player &player) const
{
    return true;
}

vector<GridPosition> SwapNumbersPowerUp::run(Board &board, Player &player)
{
    GridPosition selection1 = board.getSelections()[0];
    GridPosition selection2 = board.getSelections()[1];
    
    BoardCell &cell1 = board.getCell(selection1);
    BoardCell &cell2 = board.getCell(selection2);

    //swap the cell's contents
    BoardCell tempCell = cell1;
    cell1 = cell2;
    cell2 = tempCell;
    
    vector<GridPosition> changes;
    changes.push_back(selection1);
    changes.push_back(selection2);
    
    return changes;
}
