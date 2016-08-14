#ifndef BLANKPART_H
#define BLANKPART_H

#include <boiler.h>

class BlankPart : public Entity, public KeyInputListener
{
public:
    BlankPart();

    void onCreate() override;
    void update() override;

    void onKeyStateChanged(const KeyInputEvent &event) override;
};

#endif /* BLANKPART_H */
