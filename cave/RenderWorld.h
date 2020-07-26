#pragma once

#include "UniformBuffer.h"

class RenderElement;

struct RenderLight
{
    glm::vec3 pos;
};

struct ViewMatrices
{
    glm::mat4 view;
    glm::mat4 projection;
};

struct View
{
    ViewMatrices viewMatrices;
    glm::ivec4 shadowAtlasParams;
};

class ShaderProgram;
class RenderWorld
{
public:
    RenderWorld();
    bool init();
    ecs3::Id createMesh(const RenderElement& element);
    void destroyMesh(ecs3::Id id);
    void transform(ecs3::Id id, glm::mat4 transform);

    ecs3::Id createLight(const RenderLight& light);
    void updateLightPos(ecs3::Id id, const glm::vec3 pos);
    void destroyLight(ecs3::Id id);

    void RenderShadowMaps();
    void Render(const ViewMatrices& viewParms);
    void RenderOpaque(ShaderProgram* prog = nullptr);
    void RenderTransparent(ShaderProgram* prog = nullptr);

    void UpdateUniforms();

private:
    ecs3::PackedArrayIndex<ecs3::Id> _meshIndex;
    ecs3::TypedData<RenderElement> _meshes;
    ecs3::TypedData<glm::mat4> _transforms;

    ecs3::PackedArrayIndex<ecs3::Id> _lightIndex;
    ecs3::TypedData<RenderLight> _lights;
    //ecs3::TypedData<TypedUniformBuffer<ViewUniform>> _lightViewUniforms; //?
    TypedUniformBuffer<glm::mat4> _modelUniform;
    TypedUniformBuffer<ViewMatrices> _viewUniform;
};