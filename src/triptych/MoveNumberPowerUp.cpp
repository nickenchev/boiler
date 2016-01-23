//
//  MoveNumberPowerUp.cpp
//  triptych
//
//  Created by Nick Enchev on 2014-10-09.
//
//

#include "MoveNumberPowerUp.h"
#include "TriptychGame.h"

MoveNumberPowerUp::MoveNumberPowerUp() : PowerUp()
{
    tier = 2;
    cost = 2;
    
    code = string("MOV");
    name = string("Move");
    help = string("Move a number");

    selectionRules.push_back(SelectionRule::create(ValueValidation::REQUIRE_NON_EMPTY));
    selectionRules.push_back(SelectionRule::create(ValueValidation::REQUIRE_EMPTY));
}

bool MoveNumberPowerUp::canRecover(const Player &player) const
{
    return false;
}

vector<GridPosition> MoveNumberPowerUp::run(Board &board, Player &player)
{
    GridPosition selection1 = board.getSelections()[0];
    GridPosition selection2 = board.getSelections()[1];
    
    BoardCell &cell1 = board.getCell(selection1);
    BoardCell &cell2 = board.getCell(selection2);
    
    //swap the cell's contents
    BoardCell tempCell = cell1;
    cell1.clear();
    cell2 = tempCell;
    
    vector<GridPosition> changes;
    changes.push_back(selection1);
    changes.push_back(selection2);
    
    return changes;
}
