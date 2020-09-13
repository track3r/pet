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

    if (!_instanceUniform.init())
    {
        return false;
    }

    if (!_shadowManager.init(500, 500, 4096, 4096))
    {
        return false;
    }

    //glGenBuffers(1, &_indirectBuffer);
    //glBindBuffer(GL_DRAW_INDIRECT_BUFFER, _indirectBuffer);
    //glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawIndirectCommand) * RenderConstrains::maxInstancesPerDraw, nullptr, GL_STREAM_DRAW);
    //glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
    _indirectBuffer.init(GpuBuffer::Indirect, sizeof(DrawIndirectCommand) * RenderConstrains::maxInstancesPerDraw);

    //glGenBuffers(1, &_instanceBuffer);
    //glBindBuffer(GL_ARRAY_BUFFER, _instanceBuffer);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(InstanceData) * RenderConstrains::maxInstancesPerDraw, nullptr, GL_STREAM_DRAW);
    _instanceBuffer.init(GpuBuffer::Vertex, sizeof(InstanceData) * RenderConstrains::maxInstancesPerDraw);

    return true;
}

void RenderWorld::render(const ViewMatrices& viewParams)
{
    if (_meshIndex.size() == 0)
    {
        return;
    }

    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, -1, "RenderWorld::render");
    Renderer* renderer = Application::getRenderer();
    RenderContext* context = &renderer->getRenderContext();

    //_viewUniform.bind(1);
    context->bindUniform(_viewUniform, 1);
    //_modelUniform.bind(2);
    context->bindUniform(_modelUniform, 2);
    //_lightUniform.bind(3);
    context->bindUniform(_lightUniform, 3);
    //_instanceUniform.bind(4);

    updateInstanceData();

    renderShadowMaps();
    //_viewUniform.bindForUpdate();
    //_viewUniform.updateFull(viewParms);
    _viewUniform.updateFull(viewParams);

    renderOpaque(context);
    renderTransparent(context);
    glPopDebugGroup();
}

void RenderWorld::updateInstanceData()
{
    RenderElement* elem = _meshes.getPtr(0);
    glm::mat4* transform = _transforms.getPtr(0);

    for (int i = 0; i < _meshIndex.size(); i++)
    {
        _instanceUniformData.instance[i].tranform = glm::mat4(transform[i]);
    }
    //glBindBuffer(GL_ARRAY_BUFFER, _instanceBuffer);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(InstanceData) * _meshIndex.size(), _instanceUniformData.instance, GL_STREAM_DRAW);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    _instanceBuffer.update(0, (uint32_t) (sizeof(InstanceData) * _meshIndex.size()), _instanceUniformData.instance);
}

void RenderWorld::updateUniforms()
{

}

void RenderWorld::setupMultidraw(RenderContext* context, bool skipTransparent)
{
    _indirectBufferData.clear();
    _indirectBufferData.reserve(_meshIndex.size());
    assert(_meshIndex.size() < RenderConstrains::maxInstancesPerDraw);

    VertexBuffer* vb = nullptr;
    IndexBuffer* ib = nullptr;
    RenderElement* elem = _meshes.getPtr(0);
    glm::mat4* transform = _transforms.getPtr(0);
    vb = elem->m_vertices;
    ib = elem->m_indices;
    int drawId = 0;
    for (int i = 0; i < _meshIndex.size(); i++)
    {
        int curDrawId = drawId++;
        if (skipTransparent && elem[i]._transparent)
        {
            continue;
        }

        _indirectBufferData.emplace_back();
        DrawIndirectCommand& cmd = _indirectBufferData.back();
        cmd.baseInstance = curDrawId;
        cmd.baseVertex = 0;
        cmd.firstIndex = elem[i]._offset;
        cmd.instanceCount = 1;
        cmd.vertexCount = elem[i]._count;
    }

    //glBindBuffer(GL_DRAW_INDIRECT_BUFFER, _indirectBuffer);
    //glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawIndirectCommand) * _indirectBufferData.size(), &_indirectBufferData[0], GL_STREAM_DRAW);
    context->bindBuffer(_indirectBuffer, GpuBuffer::Indirect);
    _indirectBuffer.update(0, (uint32_t) (sizeof(DrawIndirectCommand) * _indirectBufferData.size()), &_indirectBufferData[0]);

    //glBindVertexArray(elem[0]._vao);
    context->bindVao(elem[0]._vao);
    //glBindBuffer(GL_ARRAY_BUFFER, _instanceBuffer);
    context->bindBuffer(_instanceBuffer, GpuBuffer::Vertex);
    GLsizei vec4Size = (GLsizei)sizeof(glm::vec4);

    glEnableVertexAttribArray((int)VertexAttributeIndex::Matrix);
    glVertexAttribPointer((int)VertexAttributeIndex::Matrix, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);

    glEnableVertexAttribArray((int)VertexAttributeIndex::Matrix + 1);
    glVertexAttribPointer((int)VertexAttributeIndex::Matrix + 1, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * sizeof(glm::vec4)));

    glEnableVertexAttribArray((int)VertexAttributeIndex::Matrix + 2);
    glVertexAttribPointer((int)VertexAttributeIndex::Matrix + 2, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * sizeof(glm::vec4)));

    glEnableVertexAttribArray((int)VertexAttributeIndex::Matrix + 3);
    glVertexAttribPointer((int)VertexAttributeIndex::Matrix + 3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * sizeof(glm::vec4)));

    glVertexAttribDivisor((int)VertexAttributeIndex::Matrix, 1);
    glVertexAttribDivisor((int)VertexAttributeIndex::Matrix + 1, 1);
    glVertexAttribDivisor((int)VertexAttributeIndex::Matrix + 2, 1);
    glVertexAttribDivisor((int)VertexAttributeIndex::Matrix + 3, 1);
}

void RenderWorld::issueMultidraw(RenderContext* context, int offset, int size)
{
    GLsizei s = (GLsizei)_indirectBufferData.size();
    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, s, 0);
}

void RenderWorld::teardownMultidraw(RenderContext* context)
{
    glDisableVertexAttribArray((int)VertexAttributeIndex::Matrix);
    glDisableVertexAttribArray((int)VertexAttributeIndex::Matrix + 1);
    glDisableVertexAttribArray((int)VertexAttributeIndex::Matrix + 2);
    glDisableVertexAttribArray((int)VertexAttributeIndex::Matrix + 4);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
}

static const glm::vec3 boxForward[] = 
{
    glm::vec3(1.0, 0.0, 0.0),
    glm::vec3(-1.0, 0.0, 0.0),
    glm::vec3(0.0, 1.0, 0.0),
    glm::vec3(0.0, -1.0, 0.0),
    glm::vec3(0.0, 0.0, 1.0),
    glm::vec3(0.0, 0.0, -1.0),
};

static const glm::vec3 boxUp[] =
{
    glm::vec3(0.0, 1.0, 0.0),
    glm::vec3(0.0, 1.0, 0.0),
    glm::vec3(0.0, 0.0, 1.0),
    glm::vec3(0.0, 0.0, -1.0),
    glm::vec3(0.0, 1.0, 0.0),
    glm::vec3(0.0, 1.0, 0.0),
};

void RenderWorld::renderShadowMaps()
{
    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 2, -1, "RenderWorld::renderShadowMaps");
    Renderer* renderer = Application::getRenderer();
    RenderContext* context = &renderer->getRenderContext();
    LightUniform lightUniform = {};
    RenderLight* lights = _lights.getPtr(0);
    //ShadowRt& shadowRt = renderer->_shadow;
    _shadowManager.beginShadowPasses();
    int numLights = 0;
    if (RenderConstrains::multidraw)
    {
        setupMultidraw(context, true);
    }
    
    for (int i = 0; i < _lightIndex.size(); i++)
    {
        const RenderLight& light = lights[i];

        int sides = 6;
        for (int side = 0; side < sides; side++)
        {
            uint16_t atlasSlot = _shadowManager.beginShadowVew();
            if (atlasSlot == -1)
            {
                //no space left in shadow atlas
                break;
            }
            ViewMatrices viewParms;
            viewParms.projection = _shadowManager._projection;
            //const glm::vec3 direction = light.pos + light.direction;
            const glm::vec3 direction = light.pos + boxForward[side];
            const glm::vec3 up = boxUp[side];
            viewParms.view = glm::lookAt(light.pos, direction, up);

            LightParams& params = lightUniform.lightParams[lightUniform.numLights++];
            glm::mat3x4 t;
            params.matrix = viewParms.projection * viewParms.view;
            params.pos = glm::vec4(light.pos, 0);
            params.atlas = _shadowManager.getAtlasCoords(atlasSlot);
            renderer->getDebugDraw().renderFrustum(params.matrix);

            //_viewUniform.bindForUpdate();
            _viewUniform.updateFull(viewParms);
            
            //_viewUniform.unbindForUpdate();

            //
            //_shadowManager._renderPass._program->bind();
            context->bindProgram(*_shadowManager._renderPass._program);
            if (RenderConstrains::multidraw)
            {
                issueMultidraw(context, 0, (int)_indirectBufferData.size());
            }
            else
            {
                renderOpaque(context, _shadowManager._renderPass._program, RenderElement::NoTextures);
            }       
            
            numLights++;
        }        
    }
    lightUniform.numLights = numLights;
    if (RenderConstrains::multidraw)
    {
        teardownMultidraw(context);
    }
    
    _shadowManager._renderPass.end();
    _shadowManager._renderPass.bindTexture();
    //_lightUniform.bindForUpdate();
    _lightUniform.updateFull(lightUniform);

    //_lightUniform.unbindForUpdate();
    glPopDebugGroup();
}

void RenderWorld::renderOpaque(RenderContext* context, ShaderProgram* prog, uint8_t flags)
{
    //return;
    
    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 3, -1, "RenderWorld::renderOpaque");
    //glBindBuffer(GL_ARRAY_BUFFER, _instanceBuffer);
    context->bindBuffer(_instanceBuffer, GpuBuffer::Vertex);
    if (_meshIndex.size() == 0)
    {
        return;
    }
    
    if (prog == nullptr)
    {
        Renderer* renderer = Application::getRenderer();
        prog = renderer->getDefaultProgram();
    }

    RenderElement* elem = _meshes.getPtr(0);
    //_modelUniform.bindForUpdate();
    //CheckGlError();
    //prog->bind();
    context->bindProgram(*prog);
    //glBindBufferBase(GL_UNIFORM_BUFFER, 4, _instanceBuffer);
    context->bindUniform(_instanceBuffer, 4);
    
    for (int i = 0; i < _meshIndex.size(); i++)
    {
        if (elem[i]._transparent)
        {
            continue;
        }

        //_modelUniform.updateFull(transform[i]);
        prog->setDrawId(i);
        elem[i].render(context, flags);
        
    }
    //_modelUniform.unbindForUpdate();
    glPopDebugGroup();
}

void RenderWorld::renderTransparent(RenderContext* context, ShaderProgram* prog, uint8_t flags)
{
    return;
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
    //_modelUniform.bindForUpdate();
    //prog->bind();
    context->bindProgram(*prog);
    for (int i = 0; i < _meshIndex.size(); i++)
    {
        if (!elem[i]._transparent)
        {
            continue;
        }
        _modelUniform.updateFull(transform[i]);
        elem[i].render(context, flags);
    }
    //_modelUniform.unbindForUpdate();
    glPopDebugGroup();
}