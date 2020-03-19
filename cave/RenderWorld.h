#pragma once

class RenderElement;

class RenderWorld
{
public:
    RenderWorld();
    ecs3::Id createMesh(const RenderElement& element);
    void destroyMesh(ecs3::Id id);
    void transform(ecs3::Id id, glm::mat4 transform);

    void RenderOpaque();
    void RenderTransparent();
private:
    ecs3::PackedArrayIndex<ecs3::Id> _index;
    ecs3::TypedData<RenderElement> _meshes;
    ecs3::TypedData<glm::mat4> _transforms;
};