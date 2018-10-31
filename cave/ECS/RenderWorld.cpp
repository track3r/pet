#include "pch.h"
#include "Ecs.h"
#include "ComponentTypes.h"

#include "MeshSystem.h"

#include "RenderWorld.h"

#include "../Application.h"
#include "../Renderer.h"

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

void RenderWorld::render()
{
    auto renderer = Application::getRenderer();
    glm::mat4 transform;
    transform = glm::translate(transform, glm::vec3(0, 0, -1));
    renderer->renderCube(transform);
    
    const auto& positions = _meshSystem._positions;
    for (int i = 0; i > positions._size; i++)
    {
        renderer->renderCube(positions.get(i));
    }
}
