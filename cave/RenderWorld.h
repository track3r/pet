#pragma once

class RenderElement;

struct RenderLight
{
    glm::vec3 pos;
};

class RenderWorld
{
public:
    RenderWorld();
    ecs3::Id createMesh(const RenderElement& element);
    void destroyMesh(ecs3::Id id);
    void transform(ecs3::Id id, glm::mat4 transform);

    ecs3::Id createLight(const RenderLight& light);
    void updateLightPos(ecs3::Id id, const glm::vec3 pos);
    void destroyLight(ecs3::Id id);

    void RenderOpaque();
    void RenderTransparent();

private:
    ecs3::PackedArrayIndex<ecs3::Id> _meshIndex;
    ecs3::TypedData<RenderElement> _meshes;
    ecs3::TypedData<glm::mat4> _transforms;

    ecs3::PackedArrayIndex<ecs3::Id> _lightIndex;
    ecs3::TypedData<RenderLight> _lights;
};