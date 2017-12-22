//
//  GridPosition.cpp
//  DigTale
//
//  Created by Nick Enchev on 2014-04-28.
//
//

#include "containers/gridposition.h"

GridPosition::GridPosition() : GridPosition(1, 1)
{
}

GridPosition::GridPosition(unsigned int row, unsigned int column) : row(row), column(column)
{
}

GridPosition &GridPosition::operator=(const GridPosition &rh)
{
    this->row = rh.row;
    this->column = rh.column;
    
    return *this;
}
