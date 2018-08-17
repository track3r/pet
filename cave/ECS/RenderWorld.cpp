#include "pch.h"
#include "Ecs.h"
#include "MeshSystem.h"

#include "RenderWorld.h"

RenderWorld::RenderWorld()
{
    
}

DataWriter<Vec4, MeshId> RenderWorld::getPositions()
{
    return make_dataWriter(_meshSystem._positions, _meshSystem._index);
}

void RenderWorld::update()
{
    _meshSystem.update();
}
