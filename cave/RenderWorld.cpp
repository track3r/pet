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

/*void RenderWorld::updateLightPos(ecs3::Id id, const glm::vec3 position)
{
    int pos = _lightIndex.lookup(id);
    assert(pos != -1);
    if (pos == -1)
    {
        return;
    }

    _lights.getPtr(pos)->pos = position;
    //Renderer* renderer = Application::getRenderer();
    //renderer->setLightPos(position);//TODO fixme, assumes only one light active
    //LOG("(%f, %f, %f)", position.x, position.y, position.z);
}*/

bool RenderWorld::getLight(ecs3::Id id, RenderLight& light)
{
    int pos = _lightIndex.lookup(id);
    assert(pos != -1);
    if (pos == -1)
    {
        return false;
    }

    light = *_lights.getPtr(pos);
    return true;
}

void RenderWorld::updateLight(ecs3::Id id, const RenderLight& light)
{
    int pos = _lightIndex.lookup(id);
    assert(pos != -1);
    if (pos == -1)
    {
        return;
    }

    *_lights.getPtr(pos) = light;
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

    if (!_lightUniform.init())
    {
        return false;
    }

    if (!_shadowManager.init(500, 500, 1001, 1001))
    {
        return false;
    }

    return true;
}

void RenderWorld::render(const ViewMatrices& viewParms)
{
    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, -1, "RenderWorld::render");
    _viewUniform.bind(1);
    _modelUniform.bind(2);
    _lightUniform.bind(3);
    
    renderShadowMaps();
    _viewUniform.bindForUpdate();
    _viewUniform.updateFull(viewParms);
    
    renderOpaque();
    renderTransparent();
    glPopDebugGroup();
}

void RenderWorld::updateUniforms()
{

}

void RenderWorld::renderShadowMaps()
{
    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 2, -1, "RenderWorld::renderShadowMaps");
    Renderer* renderer = Application::getRenderer();
    LightUniform lightUniform = {};
    RenderLight* lights = _lights.getPtr(0);
    //ShadowRt& shadowRt = renderer->_shadow;
    _shadowManager.beginShadowPasses();
    for (int i = 0; i < _lightIndex.size(); i++)
    {
        const RenderLight& light = lights[0];
        //shadowRt.setPos(light.pos);
        //shadowRt.bindRt();
        uint16_t atlasSlot = _shadowManager.beginShadowVew();
        if (atlasSlot == -1)
        {
            //no space left in shadow atlas
            break;
        }
        ViewMatrices viewParms;
        viewParms.projection = _shadowManager._projection;
        viewParms.view = glm::lookAt(light.pos,
            light.pos + light.direction,
            glm::vec3(0.0f, 1.0f, 0.0f));

        lightUniform.lightParams[lightUniform.numLights].matrix = viewParms.projection * viewParms.view;
        lightUniform.lightParams[lightUniform.numLights].pos = glm::vec4(light.pos, 0);
        lightUniform.lightParams[lightUniform.numLights].atlas = _shadowManager.getAtlasCoords(atlasSlot);
        lightUniform.numLights++;

        _viewUniform.bindForUpdate();
        _viewUniform.updateFull(viewParms);
        renderOpaque(_shadowManager._renderPass._program);
        //renderTransparent(shadowRt._program);
        break;
    }

    //shadowRt.unbindRt();
    _shadowManager._renderPass.end();
    _shadowManager._renderPass.bindTexture();
    _lightUniform.updateFull(lightUniform);
    glPopDebugGroup();
}

void RenderWorld::renderOpaque(ShaderProgram* prog)
{
    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 3, -1, "RenderWorld::renderOpaque");
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

void RenderWorld::renderTransparent(ShaderProgram* prog)
{
    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 4, -1, "RenderWorld::renderTransparent");
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