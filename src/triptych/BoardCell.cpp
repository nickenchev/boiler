//
//  BoardCell.cpp
//  triptych
//
//  Created by Nick Enchev on 2014-09-24.
//
//

#include "BoardCell.h"

BoardCell::BoardCell(int value, int stage)
{
    this->value = value;
    this->stage = stage;
    matchAll = false;
}

bool BoardCell::increaseValue()
{
    bool stageIncreased = false;
    if (value == BOARD_CELL_MAX_VALUE)
    {
        value = BOARD_CELL_LOWEST_VALUE;
        if (stage < BOARD_CELL_MAX_STAGE)
        {
            stage++;
            stageIncreased = true;
        }
    }
    else
    {
        value++;
    }
    
    return stageIncreased;
}

//creates a unique ID based on the stage and value of the board cell
int BoardCell::getId() const
{
    return (BOARD_CELL_STAGE_MULTIPLIER * stage) + value;
}

std::string BoardCell::getStringRepresentation() const
{
    std::string strValue = "";
    
    if (!isEmpty())
    {
        if (matchAll)
        {
            strValue = BOARD_CELL_SPECIAL_WILDCARD;
        }
        else
        {
            strValue = std::to_string(value);
        }
    }
    
    return strValue;
}

void BoardCell::setLowest()
{
    value = BOARD_CELL_LOWEST_VALUE;
    stage = BOARD_CELL_LOWEST_STAGE;
}

int BoardCell::getScoreValue() const
{
    return (BOARD_CELL_STAGE_MULTIPLIER * (stage - 1)) + (value * stage);
}