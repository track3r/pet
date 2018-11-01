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
    transform = glm::translate(transform, glm::vec3(10, 0, 0));
    //renderer->renderCube(transform);
    
    const auto& positions = _meshSystem._positions;
    for (int i = 0; i < positions._size; i++)
    {
        auto mat1 = positions.get(i);
        auto mat2 = glm::translate(glm::mat4(), glm::vec3((float)i, 0, 0));
        assert(mat1[3].x == mat2[3].x);
        renderer->renderCube(mat1);
        //transform = glm::translate(glm::mat4(), glm::vec3((float)i, 0, 0));
        //renderer->renderCube(transform);
    }
}
