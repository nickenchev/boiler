#include <glm/glm.hpp>

#include "engine.h"
#include "sdl.h"
#include "gamepart.h"
#include "renderer.h"
#include "GameDirector.h"
#include "boardentity.h"

#include "cellentity.h"

GamePart::GamePart() : Part(), game(GameType::NORMAL, 7, 7),
                       textFont(Engine::getInstance().getFontLoader().loadBMFont("data/font.fnt"))
{
    const int screenWidth = Engine::getInstance().getScreenWidth();
    const int screenHeight = Engine::getInstance().getScreenHeight();

    // load some game assets
    bigLayers = Engine::getInstance().getSpriteLoader().loadSheet("data/big_layers.json");

    // draw the top layer
    auto topLayer = std::make_shared<Entity>(Rect(0, 0, screenWidth, 400));
    topLayer->spriteSheet = bigLayers;
    topLayer->spriteFrame = bigLayers->getFrame("topLayer_1.png");
    addEntity(topLayer);

    auto board = std::make_shared<BoardEntity>(game, Rect(0, 400, screenWidth, screenWidth));
    board->onCreate();
    addEntity(board);
}

void GamePart::showCurrentNumbers()
{
    std::cout << game.getBoard().getCurrentNumber().value << ":"
              << game.getBoard().getCurrentNumber().stage << "    Next: "
              << game.getBoard().getNextNumber().value << ":"
              << game.getBoard().getNextNumber().stage << std::endl;
}

void GamePart::start()
{
    Engine::getInstance().getRenderer().setClearColor(Color3(0.1f, 0.1f, 0.2f));

    // setup input listeners
    Engine::getInstance().addMouseListener(shared_from_this());

    // 
    game.getBoard().logBoard();
    showCurrentNumbers();
}

void GamePart::handleInput()
{
    if (Engine::getInstance().keyState(SDLK_ESCAPE))
    {
        Engine::getInstance().quit();
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
