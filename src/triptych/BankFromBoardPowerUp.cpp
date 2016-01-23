//
//  BankFromBoardPowerUp.cpp
//  triptych
//
//  Created by Nick Enchev on 2015-01-16.
//
//

#include <vector>
#include "Board.h"
#include "Player.h"
#include "BankFromBoardPowerUp.h"
#include "TriptychGame.h"

BankFromBoardPowerUp::BankFromBoardPowerUp() : PowerUp()
{
    tier = 3;
    cost = 5;
    code = string("BNK");
    name = string("Bank");
    help = string("Bank a number from\nthe board");
    
    requirements.push_back(PowerUpRequirement::BANK_SLOT);
    selectionRules.push_back(SelectionRule::create(ValueValidation::REQUIRE_NON_EMPTY));
}

bool BankFromBoardPowerUp::canRecover(const Player &player) const
{
    bool canRecover = true;
    if (!player.canBank())
    {
        canRecover = false;
    }
    
    return canRecover;
}

std::vector<GridPosition> BankFromBoardPowerUp::run(Board &board, Player &player)
{
    GridPosition selection = board.getSelections()[0];
    vector<GridPosition> cellsAffected;
    cellsAffected.push_back(selection);
    
    BoardCell &cell = board(selection);
    player.bankCell(cell);
    cell.clear();
    
    return cellsAffected;
}
