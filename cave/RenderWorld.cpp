#include "pch.h"
#include "RenderWorld.h"
#include "Application.h"

RenderWorld::RenderWorld()
    :_index(1024)
{

}

ecs3::Id RenderWorld::createMesh(const RenderElement& element)
{
    ecs3::Id ret = _index.create();
    _meshes.add(element);
    _transforms.add(glm::mat4());

    return ret;
}

void RenderWorld::destroyMesh(ecs3::Id id)
{
    int pos = _index.remove(id);
    _meshes.getPtr(pos)->~RenderElement();
    _meshes.remove(pos);    
    _transforms.remove(pos);
}

void RenderWorld::transform(ecs3::Id id, glm::mat4 transform)
{
    int pos = _index.lookup(id);
    if (pos == -1)
    {
        return;
    }

    *_transforms.getPtr(pos) = transform;
}

void RenderWorld::RenderOpaque()
{
    if (_index.size() == 0)
    {
        return;
    }
    RenderElement* elem = _meshes.getPtr(0);
    glm::mat4* tranform = _transforms.getPtr(0);
    Renderer* renderer = Application::getRenderer();
    for (int i = 0; i < _index.size(); i++)
    {
        if (elem[i]._transparent)
        {
            continue;
        }

        renderer->renderElement(*renderer->getDefaultProgram(), elem[i], tranform[i]);
    }
}

void RenderWorld::RenderTransparent()
{
    if (_index.size() == 0)
    {
        return;
    }

    RenderElement* elem = _meshes.getPtr(0);
    glm::mat4* tranform = _transforms.getPtr(0);
    Renderer* renderer = Application::getRenderer();
    for (int i = 0; i < _index.size(); i++)
    {
        if (!elem[i]._transparent)
        {
            continue;
        }

        renderer->renderElement(*renderer->getDefaultProgram(), elem[i], tranform[i]);
    }
}