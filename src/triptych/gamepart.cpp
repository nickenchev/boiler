#include "engine.h"
#include "sdl.h"
#include "gamepart.h"
#include "renderer.h"

GamePart::GamePart(Engine &engine) : Part(engine), game(GameType::NORMAL, 7, 7),
                                     boardCells(game.getBoard().getRows(),
                                                game.getBoard().getColumns())
{
    getEngine().getRenderer().setClearColor(Color3(0.2f, 0.2f, 0.2f));

    // load some game assets
    mainSheet = getEngine().getSpriteLoader().loadSheet("data/triptych.json");

    // create the board entities
    for (int r = 0; r < game.getBoard().getRows(); ++r)
    {
        for (int c = 0; c < game.getBoard().getColumns(); ++c)
        {
            auto cell = std::make_shared<Entity>(Rect(0, 0, 50, 50));
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
