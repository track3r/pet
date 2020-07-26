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
    renderer->setLightPos(position);//TODO fixme, assumes only one light active
    //LOG("(%f, %f, %f)", position.x, position.y, position.z);
}

void RenderWorld::destroyLight(ecs3::Id id)
{
    int pos = _lightIndex.remove(id);
    _lights.remove(pos);
}

bool RenderWorld::init()
{
    if (!_modelUniform.init())
    {
        return false;
    }

    if (!_viewUniform.init())
    {
        return false;
    }

    return true;
}

void RenderWorld::Render(const ViewMatrices& viewParms)
{
    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, -1, "RenderWorld::Render");
    _viewUniform.bind(1);
    _modelUniform.bind(2);
    
    RenderShadowMaps();
    _viewUniform.bindForUpdate();
    _viewUniform.updateFull(viewParms);
    
    RenderOpaque();
    RenderTransparent();
    glPopDebugGroup();
}

void RenderWorld::UpdateUniforms()
{

}

void RenderWorld::RenderShadowMaps()
{
    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 2, -1, "RenderWorld::RenderShadowMaps");
    Renderer* renderer = Application::getRenderer();
    RenderLight* lights = _lights.getPtr(0);
    ShadowRt& shadowRt = renderer->_shadow;
    for (int i = 0; i < _lightIndex.size(); i++)
    {
        const RenderLight& light = lights[0];
        shadowRt.setPos(light.pos);
        shadowRt.bindRt();

        ViewMatrices viewParms;
        viewParms.projection = shadowRt._projection;
        viewParms.view = shadowRt._transform;
        _viewUniform.bindForUpdate();
        _viewUniform.updateFull(viewParms);
        RenderOpaque(shadowRt._program);
        RenderTransparent(shadowRt._program);
        break;
    }

    shadowRt.unbindRt();
    shadowRt.bindTexture();
    glPopDebugGroup();
}

void RenderWorld::RenderOpaque(ShaderProgram* prog)
{
    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 3, -1, "RenderWorld::RenderOpaque");
    if (_meshIndex.size() == 0)
    {
        return;
    }
    Renderer* renderer = Application::getRenderer();
    if (prog == nullptr)
    {
        prog = renderer->getDefaultProgram();
    }

    RenderElement* elem = _meshes.getPtr(0);
    glm::mat4* transform = _transforms.getPtr(0);
    _modelUniform.bindForUpdate();
    prog->bind();

    for (int i = 0; i < _meshIndex.size(); i++)
    {
        if (elem[i]._transparent)
        {
            continue;
        }

        _modelUniform.updateFull(transform[i]);
        elem[i].render();
        
    }
    _modelUniform.unbindForUpdate();
    glPopDebugGroup();
}

void RenderWorld::RenderTransparent(ShaderProgram* prog)
{
    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 4, -1, "RenderWorld::RenderTransparent");
    if (_meshIndex.size() == 0)
    {
        return;
    }

    RenderElement* elem = _meshes.getPtr(0);
    glm::mat4* transform = _transforms.getPtr(0);
    Renderer* renderer = Application::getRenderer();
    if (prog == nullptr)
    {
        prog = renderer->getDefaultProgram();
    }
    _modelUniform.bindForUpdate();
    prog->bind();
    for (int i = 0; i < _meshIndex.size(); i++)
    {
        if (!elem[i]._transparent)
        {
            continue;
        }
        _modelUniform.updateFull(transform[i]);
        elem[i].render();
    }
    _modelUniform.unbindForUpdate();
    glPopDebugGroup();
}