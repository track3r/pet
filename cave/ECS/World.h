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
    void update();
    
private:
    void initSystems();
};
