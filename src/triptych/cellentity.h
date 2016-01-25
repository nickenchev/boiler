#ifndef CELLENTITY_H
#define CELLENTITY_H

#include "entity.h"
#include "GridPosition.h"

class CellEntity : public Entity
{
    int row, col;
public:
    CellEntity(int row, int col, const Rect rect) : Entity(rect) { this->row = row; this->col = col; }

    int getRow() const { return row; }
    int getColumn() const { return col; }

    GridPosition getGridPosition() const { return GridPosition(row, col); }
};


#endif /* CELLENTITY_H */
