#ifndef GAMEPART_H
#define GAMEPART_H

#include <memory>
#include "part.h"
#include "bmfont.h"
#include "../input/mouseinputlistener.h"
#include "../input/mousebuttonevent.h"
#include "TriptychGame.h"

class TriptychGame;

class GamePart : public Part, public MouseInputListener, public std::enable_shared_from_this<GamePart>
{
    TriptychGame game;
    std::shared_ptr<SpriteSheet> bigLayers;
    BMFont textFont;

    void showCurrentNumbers();

public:
    GamePart();

    void start() override;
    void handleInput() override;
    void update(const float delta) override;

    void onMouseMove() override;
    void onMouseButton(const MouseButtonEvent event) override;
};


#endif /* GAMEPART_H */
