//
//  DeleteNumberPowerUp.cpp
//  triptych
//
//  Created by Nick Enchev on 2014-10-07.
//
//

#include "DeleteNumberPowerUp.h"

DeleteNumberPowerUp::DeleteNumberPowerUp() : PowerUp()
{
    tier = 1;
    cost = 2;
    
    code = string("DEL");
    name = string("Delete");
    help = string("Delete a number");
    
    selectionRules.push_back(SelectionRule::create(ValueValidation::REQUIRE_NON_EMPTY));
}

bool DeleteNumberPowerUp::canRecover(const Player &player) const
{
    return true;
}

vector<GridPosition> DeleteNumberPowerUp::run(Board &board, Player &player)
{
    GridPosition selection1 = board.getSelections()[0];
    
    BoardCell &cell1 = board.getCell(selection1);
    cell1.clear();
    
    return vector<GridPosition>();
}
