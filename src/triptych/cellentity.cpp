#include "cellentity.h"

CellEntity::CellEntity(int row, int col, const Rect rect) : Entity(rect)
{
    this->row = row;
    this->col = col;

    int cellWidth = getFrame().size.getWidth();
    int cellHeight = getFrame().size.getHeight();
    number = std::make_shared<Entity>(Rect(cellWidth / 2, cellHeight / 2, cellWidth / 2, cellHeight / 2));
}

void CellEntity::onCreate()
{
    addChild(number);
}
