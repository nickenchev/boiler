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
    const Size screenSize = Engine::getInstance().getScreenSize();

    // load some game assets
    bigLayers = Engine::getInstance().getSpriteLoader().loadSheet("data/big_layers.json");

    // draw the top layer
    auto topLayer = std::make_shared<Entity>(Rect(0, 0, screenSize.getWidth(), 400));
    topLayer->spriteSheet = bigLayers;
    topLayer->spriteFrame = bigLayers->getFrame("topLayer_1.png");
    addEntity(topLayer);

    const Size boardSize(screenSize.getWidth() * 0.8f, screenSize.getWidth() * 0.8f);
    auto board = std::make_shared<BoardEntity>(game, Rect(0, 405, boardSize));
                                                    
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
    showCurrentNumbers();
}
