#ifndef CELLENTITY_H
#define CELLENTITY_H

#include "entity.h"
#include "GridPosition.h"

class CellEntity : public Entity
{
    int row, col;
    std::shared_ptr<Entity> number;

public:
    CellEntity(int row, int col, const Rect rect);

    void onCreate() override;

    int getRow() const { return row; }
    int getColumn() const { return col; }

    GridPosition getGridPosition() const { return GridPosition(row, col); }
    std::shared_ptr<Entity> &getNumberSprite() { return number; }
};


#endif /* CELLENTITY_H */
