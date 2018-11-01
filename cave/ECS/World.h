#pragma once

class EntityWorld;
class RenderWorld;

class World
{
public:
    EntityWorld* _entityWorld;
    RenderWorld* _renderWorld;
    
    World();
    
    void testEntities();
    void update(float dt);
    void render();
    
private:
    void initSystems();
    void copyTransform();
    
    EntityId createTestEntity(glm::vec3 pos);
};
