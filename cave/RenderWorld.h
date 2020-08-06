#pragma once

#include "UniformBuffer.h"
#include "RenderShadow.h"

class RenderElement;

struct RenderLight
{
    glm::vec3 pos;
    glm::vec3 direction;
};

struct ViewMatrices
{
    glm::mat4 view;
    glm::mat4 projection;
};

struct ViewUniform
{
    ViewMatrices viewMatrices;
};

struct LightParams
{
    glm::mat4 matrix;
    glm::vec4 pos;
    glm::vec4 atlas;
};

struct LightUniform
{
    LightParams lightParams[100];
    int numLights;
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
    bool getLight(ecs3::Id id, RenderLight& light);
    void updateLight(ecs3::Id id, const RenderLight& light);
    void destroyLight(ecs3::Id id);

    void renderShadowMaps();
    void render(const ViewMatrices& viewParms);
    void renderOpaque(ShaderProgram* prog = nullptr, uint8_t flags = 0);
    void renderTransparent(ShaderProgram* prog = nullptr, uint8_t flags = 0);

    void updateUniforms();

private:
    ecs3::PackedArrayIndex<ecs3::Id> _meshIndex;
    ecs3::TypedData<RenderElement> _meshes;
    ecs3::TypedData<glm::mat4> _transforms;

    ecs3::PackedArrayIndex<ecs3::Id> _lightIndex;
    ecs3::TypedData<RenderLight> _lights;
    //ecs3::TypedData<TypedUniformBuffer<ViewUniform>> _lightViewUniforms; //?
    TypedUniformBuffer<glm::mat4> _modelUniform;
    TypedUniformBuffer<ViewMatrices> _viewUniform;
    TypedUniformBuffer<LightUniform> _lightUniform;

    ShadowManager _shadowManager;
};