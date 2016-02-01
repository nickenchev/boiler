#ifndef BOARDENTITY_H
#define BOARDENTITY_H

#include "entity.h"
#include "Array2D.h"

class TriptychGame;

class BoardEntity : public Entity
{
    TriptychGame &game;
    int rows, columns;
    ensoft::Array2D<std::shared_ptr<Entity>> boardCells;
    std::shared_ptr<SpriteSheet> triptych;
    std::shared_ptr<SpriteSheet> numStage;

public:
    BoardEntity(TriptychGame &game, const Rect rect);

    void onCreate() override;
};


#endif /* BOARDENTITY_H */
