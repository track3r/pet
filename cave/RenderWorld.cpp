#include "pch.h"
#include "RenderWorld.h"
#include "Application.h"

RenderWorld::RenderWorld()
    :_meshIndex(1024)
    ,_lightIndex(1024)
{

}

ecs3::Id RenderWorld::createMesh(const RenderElement& element)
{
    ecs3::Id ret = _meshIndex.create();
    _meshes.add(element);
    _transforms.add(glm::mat4());

    return ret;
}

void RenderWorld::destroyMesh(ecs3::Id id)
{
    int pos = _meshIndex.remove(id);
    _meshes.getPtr(pos)->~RenderElement();
    _meshes.remove(pos);    
    _transforms.remove(pos);
}

void RenderWorld::transform(ecs3::Id id, glm::mat4 transform)
{
    int pos = _meshIndex.lookup(id);
    if (pos == -1)
    {
        return;
    }

    *_transforms.getPtr(pos) = transform;
}

ecs3::Id RenderWorld::createLight(const RenderLight& light)
{
    ecs3::Id ret = _lightIndex.create();
    _lights.add(light);

    return ret;
}

void RenderWorld::updateLightPos(ecs3::Id id, const glm::vec3 position)
{
    int pos = _lightIndex.lookup(id);
    assert(pos != -1);
    if (pos == -1)
    {
        return;
    }

    _lights.getPtr(pos)->pos = position;
    Renderer* renderer = Application::getRenderer();
    renderer->setLightPos(position);//TODO fixme
    //LOG("(%f, %f, %f)", position.x, position.y, position.z);
}

void RenderWorld::destroyLight(ecs3::Id id)
{
    int pos = _lightIndex.remove(id);
    _lights.remove(pos);
}

void RenderWorld::RenderShadowMaps()
{
    for (int i = 0; i < _lightIndex.size(); i++)
    {

    }
}

void RenderWorld::RenderOpaque()
{
    if (_meshIndex.size() == 0)
    {
        return;
    }
    RenderElement* elem = _meshes.getPtr(0);
    glm::mat4* tranform = _transforms.getPtr(0);
    Renderer* renderer = Application::getRenderer();
    for (int i = 0; i < _meshIndex.size(); i++)
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
    if (_meshIndex.size() == 0)
    {
        return;
    }

    RenderElement* elem = _meshes.getPtr(0);
    glm::mat4* tranform = _transforms.getPtr(0);
    Renderer* renderer = Application::getRenderer();
    for (int i = 0; i < _meshIndex.size(); i++)
    {
        if (!elem[i]._transparent)
        {
            continue;
        }

        renderer->renderElement(*renderer->getDefaultProgram(), elem[i], tranform[i]);
    }
}