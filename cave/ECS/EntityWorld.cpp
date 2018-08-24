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
    _parentChildSystem =  new TransformSystem();
    _systems[ParentChildType] = new TransformSystem();
}

ComponentId EntityWorld::addComponent(EntityId entityId, ComponentTypeId componentType)
{
    ComponentId comp = _systems[componentType]->createComponent();
    Entity* entity = get(entityId);
    entity->addComponent(comp, componentType);
    
    return comp;
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
    
    ComponentId childCompId = childEnt->getComponent(ParentChildType);
    if (childCompId == invalid<ComponentId>())
    {
        childCompId = _systems[ParentChildType]->createComponent();
        childEnt->addComponent(childCompId, ParentChildType);
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
            oldParentCompId = oldParentEnt->getComponent(ParentChildType);
        }
    }
    
    ComponentId newParentCompId;
    if (parent != invalid<EntityId>())
    {
        Entity* parentEnt = get(parent);
        newParentCompId = parentEnt->getComponent(ParentChildType);
    }
    
    _parentChildSystem->setParent(childCompId, child, oldParentCompId, newParentCompId, parent);
}

EntityId EntityWorld::create()
{
    EntityId id = _index.create();
    Entity empty;
    _entities.add(empty);
    //_parents.add(invalid<EntityId>());
    
    return id;
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

void calculateGlobalPos();
