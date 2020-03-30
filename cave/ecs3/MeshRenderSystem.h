#pragma once

class RenderElement;
class RenderWorld;

struct RenderSingleton
{
    RenderWorld* world;
};

class MeshComponent
{
public:
    ~MeshComponent();
    static const int ID = (int)ecs3::ComponentType::Mesh;
    ecs3::Id mesh = ecs3::invalid<ecs3::Id>();
};

class LightComponent
{
public:
    LightComponent()
    {

    }
    LightComponent(ecs3::Id id)
        :light(id)
    {

    }
    ~LightComponent();
    static const int ID = (int)ecs3::ComponentType::Light;
    ecs3::Id light = ecs3::invalid<ecs3::Id>();
};

class MeshRenderSystem final : public ecs3::System
{
public:
    MeshRenderSystem(ecs3::World* world);
    virtual void onRegister() override;
    virtual void onBeforeUpdate() override;
    virtual void onUpdate(ecs3::BlockIterator& iterator) override;
};

class LightRenderSystem final : public ecs3::System
{
public:
    LightRenderSystem(ecs3::World* world);
    virtual void onRegister() override;
    virtual void onBeforeUpdate() override;
    virtual void onUpdate(ecs3::BlockIterator& iterator) override;
};