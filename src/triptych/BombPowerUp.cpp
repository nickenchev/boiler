//
//  BombPowerUp.cpp
//  triptych
//
//  Created by Nick Enchev on 2014-10-07.
//
//

#include "BombPowerUp.h"

BombPowerUp::BombPowerUp() : PowerUp()
{
    tier = 3;
    cost = 5;
    code = string("BMB");
    name = string("Bomb");
    help = string("Blow up 9 tiles");
    
    selectionRules.push_back(SelectionRule::create(ValueValidation::NO_REQUIREMENT));
}

bool BombPowerUp::canRecover(const Player &player) const
{
    return true;
}

vector<GridPosition> BombPowerUp::run(Board &board, Player &player)
{
    vector<GridPosition> cellsAffected;

    GridPosition selection = board.getSelections()[0];
    board.getCell(selection).clear();

    auto &cells = board.getCells();
    GridPosition adj;
    if (cells.getLeft(selection, adj))
    {
        board.getCell(adj).clear();
        cellsAffected.push_back(adj);
    }
    if (cells.getTopLeft(selection, adj))
    {
        board.getCell(adj).clear();
        cellsAffected.push_back(adj);
    }
    if (cells.getTop(selection, adj))
    {
        board.getCell(adj).clear();
        cellsAffected.push_back(adj);
    }
    if (cells.getTopRight(selection, adj))
    {
        board.getCell(adj).clear();
        cellsAffected.push_back(adj);
    }
    if (cells.getRight(selection, adj))
    {
        board.getCell(adj).clear();
        cellsAffected.push_back(adj);
    }
    if (cells.getBottomRight(selection, adj))
    {
        board.getCell(adj).clear();
        cellsAffected.push_back(adj);
    }
    if (cells.getBottom(selection, adj))
    {
        board.getCell(adj).clear();
        cellsAffected.push_back(adj);
    }
    if (cells.getBottomLeft(selection, adj))
    {
        board.getCell(adj).clear();
        cellsAffected.push_back(adj);
    }
    
    return cellsAffected;
}
