#ifndef TERRAINPART_H
#define TERRAINPART_H

#include <memory>
#include <entity.h>
#include <keyinputlistener.h>
#include <array2d.h>

class TerrainPart : public Entity, public KeyInputListener
{
    std::shared_ptr<SpriteSheet> terrainSheet;

public:
    TerrainPart();

    void onCreate() override;

    void onKeyStateChanged(const KeyInputEvent &event) override;
};


#endif /* TERRAINPART_H */
