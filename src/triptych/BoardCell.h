//
//  BoardCell.h
//  triptych
//
//  Created by Nick Enchev on 2014-09-24.
//
//

#ifndef __triptych__BoardCell__
#define __triptych__BoardCell__

#include <iostream>
#include "GridPosition.h"

#define BOARD_CELL_SPECIAL_WILDCARD "W"
#define BOARD_CELL_LOWEST_STAGE 1
#define BOARD_CELL_MAX_STAGE 5
#define BOARD_CELL_MAX_VALUE 9
#define BOARD_CELL_LOWEST_VALUE 1
#define BOARD_CELL_STAGE_MULTIPLIER 10

class BoardCell
{
public:
    int value = 0;
    int stage = BOARD_CELL_LOWEST_STAGE;
    GridPosition position;
    bool matchAll;
    
    BoardCell() : BoardCell(0, BOARD_CELL_LOWEST_STAGE) { }
    BoardCell(int value, int stage);

    bool operator==(const BoardCell &rh) const
    {
        bool isSame = ((this->value == rh.value && this->stage == rh.stage) ||
                       ((matchAll || rh.matchAll) && (!isEmpty() && !rh.isEmpty())));
        return isSame;
    }

    bool operator<=(const BoardCell &rh) const
    {
        bool isLessThan = false;
        if (stage == rh.stage)
        {
            isLessThan = value <= rh.value;
        }
        else if (stage < rh.stage)
        {
            isLessThan = true;
        }
        
        return isLessThan;
    }
    bool operator>(const BoardCell &rh) const
    {
        bool greaterThan = false;
        if (stage > rh.stage)
        {
            greaterThan = true;
        }
        else if (stage == rh.stage && value > rh.value)
        {
            greaterThan = true;
        }
        
        return greaterThan;
    }
    bool operator<(const BoardCell &rh) const
    {
        bool lessThan = false;
        if (stage < rh.stage)
        {
            lessThan = true;
        }
        else if (stage == rh.stage && value < rh.value)
        {
            lessThan = true;
        }
        
        return lessThan;
    }
    
    void operator=(const BoardCell &rh)
    {
        value = rh.value;
        stage = rh.stage;
        matchAll = rh.matchAll;
    }

    bool isEmpty() const { return (value == 0); }
    void setLowest();
    void clear() { value = 0; stage = BOARD_CELL_LOWEST_STAGE; matchAll = false; }
    bool increaseValue();
    int getScoreValue() const;
    int getId() const;
    std::string getStringRepresentation() const;
};

#endif /* defined(__triptych__BoardCell__) */
