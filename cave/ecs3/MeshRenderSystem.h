#pragma once

class RenderElement;
class RenderWorld;

struct RenderSingleton
{
    RenderWorld* world;
};

class MeshComponent : public ecs3::AutoRegisterComponent<MeshComponent, (int)ecs3::ComponentType::Mesh>
{
public:
    ~MeshComponent();
    ecs3::Id mesh = ecs3::invalid<ecs3::Id>();
};

class LightComponent : public ecs3::AutoRegisterComponent<LightComponent, (int)ecs3::ComponentType::Light>
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
    ecs3::Id light = ecs3::invalid<ecs3::Id>();
};

class MeshRenderSystem final : public ecs3::System
{
public:
    MeshRenderSystem(ecs3::World* world);
    virtual void onRegister() override;
    virtual void onBeforeUpdate() override;
    virtual void onUpdate(ecs3::BlockIterator& iterator) override;
    virtual void onCreateEntity(ecs3::Id entId, ecs3::EntityAccessor& entity) override;
    virtual void onDeleteEntity(ecs3::Id entId, ecs3::EntityAccessor& entity) override;
};

class LightRenderSystem final : public ecs3::System
{
public:
    LightRenderSystem(ecs3::World* world);
    virtual void onRegister() override;
    virtual void onBeforeUpdate() override;
    virtual void onUpdate(ecs3::BlockIterator& iterator) override;
};