#pragma once

class RenderWorld
{
public:
    MeshSystem _meshSystem;
    
    RenderWorld();
    DataWriter<Vec4, MeshId> getPositions();
    void update();
};
