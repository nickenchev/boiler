#ifndef BLANKPART_H
#define BLANKPART_H

#include <boiler.h>

class BlankPart : public Entity
{
public:
    BlankPart();

    void onCreate() override;
    void update() override;
};

#endif /* BLANKPART_H */
