#include "boardentity.h"
#include "engine.h"
#include "TriptychGame.h"
#include "cellentity.h"

BoardEntity::BoardEntity(TriptychGame &game, const Rect rect) : Entity(rect),
                                                                      game(game),
                                                                      boardCells(6, 6)
{
    this->rows = game.getBoard().getRows();
    this->columns = game.getBoard().getColumns();

    triptych = Engine::getInstance().getSpriteLoader().loadSheet("data/triptych.json");
    numStage = Engine::getInstance().getSpriteLoader().loadSheet("data/num_stage_1.json");
}

void BoardEntity::onCreate()
{
    const Board &board = game.getBoard();
    // create the board entities
    int cellWidth = frame.size.getWidth() / board.getRows();
    int cellHeight = frame.size.getHeight() / board.getColumns();
    int xOffset = frame.position.x;
    int yOffset = frame.position.y;
    for (int r = 1; r <= game.getBoard().getRows(); ++r)
    {
        for (int c = 1; c <= game.getBoard().getColumns(); ++c)
        {
            int x = (c - 1) * cellWidth + xOffset;
            int y = (r - 1) * cellHeight + yOffset;
            auto cell = std::make_shared<CellEntity>(r, c, Rect(x, y, cellWidth, cellHeight));

            cell->spriteSheet = triptych;
            cell->spriteFrame = triptych->getFrame("tile_1.png");
            addChild(cell);
        }
    }
}
