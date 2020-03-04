#pragma once

class PlayerComponent
{
public:
    static const int ID = (int)ecs3::ComponentType::Player;
};

class PlayerSysten final : public ecs3::System
{
public:
    PlayerSysten(ecs3::World* world);
    virtual void onRegister() override;
    virtual void onBeforeUpdate() override;
    virtual void onUpdate(ecs3::BlockIterator& iterator) override;
};