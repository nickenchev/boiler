#ifndef TERRAINPART_H
#define TERRAINPART_H

#include <memory>
#include <entity.h>
#include <keyinputlistener.h>
#include <array2d.h>

class Camera;

class TerrainPart : public Entity, public KeyInputListener
{
    std::shared_ptr<const SpriteSheet> terrainSheet;
    std::shared_ptr<const SpriteSheet> procSheet;
    std::shared_ptr<Camera> camera;
    bool keys[255];
    glm::vec3 cameraMove;

public:
    TerrainPart();

    void onCreate() override;
    void update() override;

    void onKeyStateChanged(const KeyInputEvent &event) override;
};


#endif /* TERRAINPART_H */
