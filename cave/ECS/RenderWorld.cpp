#include "pch.h"
#include "Ecs.h"
#include "ComponentTypes.h"

#include "MeshSystem.h"

#include "RenderWorld.h"

RenderWorld::RenderWorld()
{
    
}

DataWriter<glm::mat4, MeshId> RenderWorld::getPositions()
{
    return make_dataWriter(_meshSystem._positions, _meshSystem._index);
}

void RenderWorld::update(float dt)
{
    _meshSystem.update(dt);
}
