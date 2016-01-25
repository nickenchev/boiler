#include <glm/glm.hpp>

#include "engine.h"
#include "sdl.h"
#include "gamepart.h"
#include "renderer.h"
#include "GameDirector.h"

#include "cellentity.h"

GamePart::GamePart(Engine &engine) : Part(engine), game(GameType::NORMAL, 7, 7),
                                     boardCells(game.getBoard().getRows(),
                                                game.getBoard().getColumns())
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

GamePart::~GamePart()
{
}

void GamePart::start()
{
    game.getBoard().logBoard();

    getEngine().getRenderer().setClearColor(Color3(0.1f, 0.1f, 0.2f));

    // setup input listeners
    getEngine().addMouseListener(shared_from_this());
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
                const bool usingBank = game.getPlayer().bankSlotSelected();
                const BoardCell &currentNumber = (usingBank) ? game.getPlayer().getSelectedBankCell() : game.getBoard().takeNumber();
                game.turnBegin();

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
            }
        }
    }
}
