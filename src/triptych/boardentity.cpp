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
    Engine::getInstance().addMouseListener(shared_from_this());
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

void BoardEntity::onMouseMove()
{
}

void BoardEntity::onMouseButton(const MouseButtonEvent event)
{
    if (event.button == MouseButton::LEFT && event.state == ButtonState::UP)
    {
        auto entities = getChildren();
        for (auto itr = entities.begin(); itr != entities.end(); ++itr)
        {
            // grab the cell entity and check if it was selected
            const auto &cellEntity = std::static_pointer_cast<CellEntity>(*itr);
            if (cellEntity->frame.collides(glm::vec2(getX(), getY())))
            {
                BoardCell &boardCell = game.getBoard().getCell(cellEntity->getRow(), cellEntity->getColumn());
                if (boardCell.isEmpty())
                {
                    game.turnBegin();

                    const bool usingBank = game.getPlayer().bankSlotSelected();
                    const BoardCell currentNumber = (usingBank) ? game.getPlayer().getSelectedBankCell() : game.getBoard().takeNumber();
                    boardCell = currentNumber;

                    long placementScore = 0;
                    do
                    {
                        Triptych triptych(TriptychDirection::NONE);
                        placementScore = game.getBoard().scorePlacement(cellEntity->getGridPosition(), triptych);
                        if (placementScore)
                        {
                            std::cout << placementScore << std::endl;
                            game.getBoard().logBoard();
                        }
                    } while (placementScore > 0);

                    TurnInfo turnInfo = game.turnEnd();

                    game.getBoard().logBoard();
                }
            }
        }
    }
}
