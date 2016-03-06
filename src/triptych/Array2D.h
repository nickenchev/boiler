//
//  Array2D.h
//  DigTale
//
//  Created by Nick Enchev on 2014-04-18.
//
//

#ifndef __ENSOFT__Array2D__
#define __ENSOFT__Array2D__

#include <vector>
#include "GridPosition.h"

namespace ensoft
{
    template <class T> class Array2D
    {
        unsigned int rows, columns, size;
        std::vector<T> items;

    public:
        Array2D(unsigned int rows, unsigned int columns) : items(rows * columns)
        {
            this->size = rows * columns;
            this->rows = rows;
            this->columns = columns;
        }
        
        //used for iterating over all of the elements in a 1D format
        T &operator[](unsigned int index)
        {
            return items[index];
        }
        T &operator()(GridPosition pos)
        {
            return (*this)(pos.row, pos.column);
        }

        //used for setting int the 2D format
        T &operator()(unsigned int row, unsigned int column)
        {
            int rowNumber = (row * this->columns) - (columns - column);
            T &obj = items[rowNumber - 1];
            return obj;
        }

        unsigned int getSize() const { return this->size; }
        unsigned int getRows() const { return this->rows; }
        unsigned int getColumns() const { return this->columns; }

        bool validPosition(GridPosition position) const
        {
            return (position.row >= 1 && position.row <= rows) && (position.column >= 1 && position.column <= columns);
        }
        
        bool getTop(GridPosition position, GridPosition &newPosition) const
        {
            newPosition.row = position.row - 1;
            newPosition.column = position.column;
            return validPosition(newPosition);
        }
        bool getBottom(GridPosition position, GridPosition &newPosition) const
        {
            newPosition.row = position.row + 1;
            newPosition.column = position.column;
            return validPosition(newPosition);
        }
        bool getLeft(GridPosition position, GridPosition &newPosition) const
        {
            newPosition.row = position.row;
            newPosition.column = position.column - 1;
            return validPosition(newPosition);
        }
        bool getRight(GridPosition position, GridPosition &newPosition) const
        {
            newPosition.row = position.row;
            newPosition.column = position.column + 1;
            return validPosition(newPosition);
        }
        bool getTopLeft(GridPosition position, GridPosition &newPosition) const
        {
            newPosition.row = position.row - 1;
            newPosition.column = position.column - 1;
            return validPosition(newPosition);
        }
        bool getTopRight(GridPosition position, GridPosition &newPosition) const
        {
            newPosition.row = position.row - 1;
            newPosition.column = position.column + 1;
            return validPosition(newPosition);
        }
        bool getBottomLeft(GridPosition position, GridPosition &newPosition) const
        {
            newPosition.row = position.row + 1;
            newPosition.column = position.column - 1;
            return validPosition(newPosition);
        }
        bool getBottomRight(GridPosition position, GridPosition &newPosition) const
        {
            newPosition.row = position.row + 1;
            newPosition.column = position.column + 1;
            return validPosition(newPosition);
        }
    };
}

#endif /* defined(__DigTale__Array2D__) */
