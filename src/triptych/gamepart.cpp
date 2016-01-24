#include "engine.h"
#include "sdl.h"
#include "gamepart.h"
#include "renderer.h"

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
    for (int r = 0; r < game.getBoard().getRows(); ++r)
    {
        for (int c = 0; c < game.getBoard().getColumns(); ++c)
        {
            int x = c * cellWidth + xOffset;
            int y = r * cellHeight + yOffset;
            auto cell = std::make_shared<Entity>(Rect(x, y, cellWidth, cellHeight));

            cell->spriteSheet = mainSheet;
            cell->spriteFrame = mainSheet->getFrame("menu_1.png");
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

    getEngine().getRenderer().setClearColor(Color3(0.1f, 0.1f, 0.3f));

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
    game.turnBegin();
    TurnInfo turnInfo = game.turnEnd();

    game.getBoard().logBoard();
}
