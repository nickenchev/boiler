#ifndef GAMEPART_H
#define GAMEPART_H

#include <memory>
#include "part.h"
#include "TriptychGame.h"
#include "Array2D.h"

class TriptychGame;

class GamePart : public Part
{
    TriptychGame game;
    std::shared_ptr<SpriteSheet> mainSheet;
    ensoft::Array2D<std::shared_ptr<Entity>> boardCells;

public:
    GamePart(Engine &engine);
    virtual ~GamePart();

    void start() override;
    void handleInput() override;
    void update(const float delta) override;
};


#endif /* GAMEPART_H */
