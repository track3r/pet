#include "pch.h"
#include "TileGrid.h"

struct GridInfo_t
{
    int width;
    int height;
};

TileGridSystem::TileGridSystem(ecs3::World* world)
    :ecs3::System(world)
{
}

void TileGridSystem::onRegister()
{
    addComponent<ecs3::TransformComponent>();
    addComponent<TileGridComponent>();
}

void TileGridSystem::onBeforeUpdate()
{
}

void TileGridSystem::onUpdate(ecs3::BlockIterator& iterator)
{

}

void TileGridSystem::onCreateEntity(ecs3::Id entId, ecs3::EntityAccessor& entity)
{
}

void TileGridSystem::onDeleteEntity(ecs3::Id entId, ecs3::EntityAccessor& entity)
{
}
