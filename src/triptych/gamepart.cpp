#include <glm/glm.hpp>

#include "engine.h"
#include "gamepart.h"
#include "renderer.h"
#include "GameDirector.h"
#include "boardentity.h"
#include "cellentity.h"

GamePart::GamePart() : game(GameType::NORMAL, 7, 7),
                       textFont(Engine::getInstance().getFontLoader().loadBMFont("data/font.fnt"))
{
    // load some game assets
    bigLayers = Engine::getInstance().getSpriteLoader().loadSheet("data/big_layers.json");
}

void GamePart::onCreate()
{
    const Size screenSize = Engine::getInstance().getScreenSize();

    // draw the top layer
    auto topLayer = std::make_shared<Entity>(Rect(0, 0, screenSize.getWidth(), 400));
    topLayer->spriteFrame = bigLayers->getFrame("topLayer_1.png");
    addChild(topLayer);

    const Size boardSize(screenSize.getWidth() * 0.8f, screenSize.getWidth() * 0.8f);
    auto board = std::make_shared<BoardEntity>(game, Rect(0, 405, boardSize));

    addChild(board);

    Engine::getInstance().getRenderer().setClearColor(Color3(0.1f, 0.1f, 0.2f));

    // setup input listeners
    Engine::getInstance().addMouseListener(this);

    game.getBoard().logBoard();
    showCurrentNumbers();
}

void GamePart::showCurrentNumbers()
{
    std::cout << game.getBoard().getCurrentNumber().value << ":"
              << game.getBoard().getCurrentNumber().stage << "    Next: "
              << game.getBoard().getNextNumber().value << ":"
              << game.getBoard().getNextNumber().stage << std::endl;
}

void GamePart::onMouseMove()
{
}
void GamePart::onMouseButton(const MouseButtonEvent event)
{
    if (event.button == MouseButton::RIGHT)
    {
        Engine::getInstance().quit();
    }
    else
    {
        showCurrentNumbers();
    }
}
