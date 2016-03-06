#ifndef BOARDENTITY_H
#define BOARDENTITY_H

#include "entity.h"
#include "Array2D.h"
#include "../input/mouseinputlistener.h"
#include "../input/mousebuttonevent.h"

class TriptychGame;

class BoardEntity : public Entity, public MouseInputListener 
{
    TriptychGame &game;
    int rows, columns;
    ensoft::Array2D<std::shared_ptr<Entity>> boardCells;
    std::shared_ptr<SpriteSheet> triptych;
    std::shared_ptr<SpriteSheet> numStage;

public:
    BoardEntity(TriptychGame &game, const Rect &rect);

    void onCreate() override;
    void onMouseMove() override;
    void onMouseButton(const MouseButtonEvent event) override;

    void refreshBoard();
};


#endif /* BOARDENTITY_H */
