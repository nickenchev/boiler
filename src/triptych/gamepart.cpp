#include <glm/glm.hpp>

#include "engine.h"
#include "sdl.h"
#include "gamepart.h"
#include "renderer.h"
#include "GameDirector.h"

#include "cellentity.h"

GamePart::GamePart(Engine &engine) : Part(engine), game(GameType::NORMAL, 7, 7),
                                     boardCells(game.getBoard().getRows(),
                                                game.getBoard().getColumns()),
                                     textFont(getEngine().getFontLoader().loadBMFont("data/font.fnt"))
{
    // load some game assets
    mainSheet = getEngine().getSpriteLoader().loadSheet("data/triptych.json");

    // create the board entities
    int cellWidth = 75;
    int cellHeight = 75;
    int xOffset = 30;
    int yOffset = 30;
    for (int r = 1; r <= game.getBoard().getRows(); ++r)
    {
        for (int c = 1; c <= game.getBoard().getColumns(); ++c)
        {
            int x = (c - 1) * cellWidth + xOffset;
            int y = (r - 1) * cellHeight + yOffset;
            auto cell = std::make_shared<CellEntity>(r, c, Rect(x, y, cellWidth, cellHeight));

            cell->spriteSheet = mainSheet;
            cell->spriteFrame = mainSheet->getFrame("tile_1.png");
            addEntity(cell);
        }
    }
}

void GamePart::showCurrentNumbers()
{
    std::cout << game.getBoard().getCurrentNumber().value << ":" << game.getBoard().getCurrentNumber().stage << "    Next: "
              << game.getBoard().getNextNumber().value << ":" << game.getBoard().getNextNumber().stage << std::endl;
}

void GamePart::start()
{
    getEngine().getRenderer().setClearColor(Color3(0.1f, 0.1f, 0.2f));

    // setup input listeners
    getEngine().addMouseListener(shared_from_this());

    // 
    game.getBoard().logBoard();
    showCurrentNumbers();
}

void GamePart::handleInput()
{
    if (getEngine().keyState(SDLK_ESCAPE))
    {
        getEngine().quit();
    }
}

void GamePart::update(const float delta)
{
}

void GamePart::onMouseMove()
{
}
void GamePart::onMouseButton(const MouseButtonEvent event)
{
    if (event.button == MouseButton::LEFT && event.state == ButtonState::UP)
    {
        auto entities = getEntities();
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
                    showCurrentNumbers();
                }
            }
        }
    }
}
