#pragma once

enum TileGridTypes
{
    TileGrid = 0,
};

class TileGridComponent : public ecs3::AutoRegisterComponent<TileGridComponent, (int)ecs3::ComponentType::User + TileGrid>
{
public:
    //ecs3::Id mesh = ecs3::invalid<ecs3::Id>();
    glm::vec2 pos;
};

class TileGridController
{
    struct tile_t
    {
        ecs3::Id id = ecs3::invalid<ecs3::Id>();
        bool blocked = false;
    };

    tile_t _tiles[32][32];

    TileGridController()
    {
        
    }
};

class TileGridSystem : public ecs3::System
{
    TileGridSystem(ecs3::World* world);
    virtual void onRegister() override;
    virtual void onBeforeUpdate() override;
    virtual void onUpdate(ecs3::BlockIterator& iterator) override;
    virtual void onCreateEntity(ecs3::Id entId, ecs3::EntityAccessor& entity) override;
    virtual void onDeleteEntity(ecs3::Id entId, ecs3::EntityAccessor& entity) override;
};