#pragma once

#include "Ecs.h"
#include "ComponentTypes.h"

struct Entity
{
    int size = 0;
    ComponentId ids[c_maxComponents];
    
    Entity()
    {
        memset(ids, -1, sizeof(ids));
    }
    
    void addComponent(ComponentId component, ComponentTypeId type)
    {
        ids[type] = component;
    }
    
    ComponentId removeComponent(ComponentTypeId type)
    {
        ComponentId id = ids[type];
        ids[type] = invalid<ComponentId>();
        return id;
    }
    
    ComponentId getComponent(ComponentTypeId type) const
    {
        return ids[type];
    }
    
    bool hasComponent(ComponentTypeId type)
    {
        return ids[type] != invalid<ComponentId>();
    }
};

void swap(Entity& a, Entity& b);

class TransformSystem;

class EntityWorld
{
public:
    System* _systems[MaxSystemsType];
    TransformSystem* _parentChildSystem;
    PackedArrayIndex<EntityId> _index;
    Data<Entity> _entities;
    
    EntityWorld();
    
    ComponentId addComponent(EntityId entityId, ComponentTypeId componentType);
    void removeComponent(EntityId entityId, ComponentTypeId componentType);
    void setParent(EntityId child, EntityId parent);
    EntityId create();
    Entity* get(EntityId id);
    void remove(EntityId id);
    void calculateGlobalPos();
};

