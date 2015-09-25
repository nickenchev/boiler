#ifndef ENTITYSTATE_H
#define ENTITYSTATE_H

class SpriteAnimation;

class EntityState 
{
    const SpriteAnimation *animation;

public:
    EntityState(SpriteAnimation *animation);

    virtual void onEnter() = 0;
    virtual void onExit() = 0;
    virtual void handleInput() = 0;
};

#endif /* ENTITYSTATE_H */
