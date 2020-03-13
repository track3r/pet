#pragma once

class RenderElement;

class MeshComponent
{
public:
    ~MeshComponent();
    static const int ID = (int)ecs3::ComponentType::Mesh;
    RenderElement* mesh = nullptr;
};

class MeshRenderSystem final : public ecs3::System
{
public:
    MeshRenderSystem(ecs3::World* world);
    virtual void onRegister() override;
    virtual void onBeforeUpdate() override;
    virtual void onUpdate(ecs3::BlockIterator& iterator) override;
};