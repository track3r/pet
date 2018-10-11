//
//  EntityWorld.cpp
//  test
//
//  Created by Alexander Demyanenko on 8/10/18.
//  Copyright © 2018 Alexander Demyanenko. All rights reserved.
//

#include "pch.h"

#include "Ecs.h"
#include "ComponentTypes.h"

#include "TransformSystem.h"
#include "EntityWorld.h"


void swap(Entity& a, Entity& b)
{
    Entity temp;
    memcpy(&temp, &a, sizeof(Entity));
    memcpy(&a, &b, sizeof(Entity));
    memcpy(&b, &temp, sizeof(Entity));
}

EntityWorld::EntityWorld()
:_index(1024)
,_entities(1024)
{
    _systems[TransformType] = new TransformSystem();
}

const ComponentId EntityWorld::addComponent(EntityId entityId, ComponentTypeId componentType)
{
    const ComponentId comp = _systems[componentType]->createComponent(entityId);
    Entity* entity = get(entityId);
    entity->addComponent(comp, componentType);
    
    return comp;
}

void EntityWorld::addComponent(EntityId entityId, ComponentTypeId componentType, ComponentId componentId)
{
    Entity* entity = get(entityId);
    entity->addComponent(componentId, componentType);
}

const ComponentId EntityWorld::getComponent(EntityId entityId, ComponentTypeId componentType) const
{
    const Entity* entity = get(entityId);
    
    if (!entity->hasComponent(componentType))
    {
        return invalid<ComponentId>();
    }
    
    const ComponentId id = entity->getComponent(componentType);
    return id;
}

const int EntityWorld::getComponentPos(EntityId entityId, ComponentTypeId componentType) const
{
    const Entity* entity = get(entityId);

    const ComponentId id = entity->getComponent(componentType);
    if (id == invalid<ComponentId>())
    {
        return -1;
    }

    return _systems[componentType]->getIndex().lookup(id);
}

void EntityWorld::removeComponent(EntityId entityId, ComponentTypeId componentType)
{
    Entity* entity = get(entityId);
    ComponentId id = entity->removeComponent(componentType);
    _systems[componentType]->removeComponent(id);
}

void EntityWorld::setParent(EntityId child, EntityId parent)
{
    Entity* childEnt = get(child);
    ComponentId oldParentCompId;
    
    ComponentId childCompId = childEnt->getComponent(TransformType);
    if (childCompId == invalid<ComponentId>())
    {
        childCompId = _systems[TransformType]->createComponent(child);
        childEnt->addComponent(childCompId, TransformType);
    }
    else
    {
        const int pos = _parentChildSystem->_index.lookup(childCompId);
        const EntityId oldParentEntId = *_parentChildSystem->_parents.getPointer(pos);
        Entity* oldParentEnt = nullptr;
        if (oldParentEntId != invalid<EntityId>())
        {
            oldParentEnt = get(oldParentEntId);
        }
        
        if (oldParentEnt != nullptr)
        {
            oldParentCompId = oldParentEnt->getComponent(TransformType);
        }
    }
    
    ComponentId newParentCompId;
    if (parent != invalid<EntityId>())
    {
        Entity* parentEnt = get(parent);
        newParentCompId = parentEnt->getComponent(TransformType);
    }
    
    _parentChildSystem->setParent(childCompId, oldParentCompId, newParentCompId, parent);
}

EntityId EntityWorld::create()
{
    EntityId id = _index.create();
    Entity empty;
    _entities.add(empty);
    //_parents.add(invalid<EntityId>());
    
    return id;
}

const Entity* EntityWorld::get(EntityId id) const
{
    if (id == invalid<EntityId>())
    {
        return nullptr;
    }
    
    int pos = _index.lookup(id);
    return _entities.getPointer(pos);
}

Entity* EntityWorld::get(EntityId id)
{
    if (id == invalid<EntityId>())
    {
        return nullptr;
    }

    int pos = _index.lookup(id);
    return _entities.getPointer(pos);
}

void EntityWorld::remove(EntityId id)
{
    int pos = _index.swapToEndAndRemove(id);
    _entities.remove(pos);
    //_parents.remove(pos);
}

void EntityWorld::update(float dt)
{
    _systems[TransformType]->update(dt);
}


GameplaySystem::GameplaySystem(EntityWorld* world, ComponentTypeId type)
    :_entityWorld(world)
    ,_componentType(type)
{
    
}

void GameplaySystem::createAndAddComponent(EntityId entity)
{
    auto componentId = createComponent(entity);
    _entityWorld->addComponent(entity, _componentType, componentId);
}
