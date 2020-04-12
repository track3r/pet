#pragma once

class PlayerComponent : public ecs3::AutoRegisterComponent<PlayerComponent, (int)ecs3::ComponentType::Player>
{
public:
};

class PlayerSystem final : public ecs3::System
{
public:
    PlayerSystem(ecs3::World* world);
    virtual void onRegister() override;
    virtual void onBeforeUpdate() override;
    virtual void onUpdate(ecs3::BlockIterator& iterator) override;
};