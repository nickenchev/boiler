#ifndef TERRAINPART_H
#define TERRAINPART_H

#include <memory>
#include <boiler.h>

class Camera;

class TerrainPart : public Entity
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

    void touchMotion(const TouchMotionEvent &event);
    void mouseMotion(const MouseMotionEvent &event);
    void keyInput(const KeyInputEvent &event);
};


#endif /* TERRAINPART_H */
