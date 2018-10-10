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
    
    bool hasComponent(ComponentTypeId type) const
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
    
    const ComponentId addComponent(EntityId entityId, ComponentTypeId componentType);
    void addComponent(EntityId entityId, ComponentTypeId componentType, ComponentId componentId);
    void removeComponent(EntityId entityId, ComponentTypeId componentType);
    const ComponentId getComponent(EntityId entityId, ComponentTypeId componentType);
    void setParent(EntityId child, EntityId parent);
    
    //Reader<T> r = getReader(ComponentTypeId, DataStreamId)
    //T val = r.get(EntityId)
    
    //T get(EntityId) {
    //ent = EntityWorld->getEnt(EntityId)
    //compId = ent->getCompId(ComponentTypeId)
    //read(compId)
    
    
    //job = Job(Read(CompA, StreamA), ReadLocal((CompA, StreamA)), Write(CompB, streamB))
    EntityId create();
    Entity* get(EntityId id);
    void remove(EntityId id);
    void calculateGlobalPos();
    
    template <typename T>
    T* getSystem()
    {
        return (T*) _systems[T::type];
    }
};

class GameplaySystem : public System
{
public:
    GameplaySystem(EntityWorld* world, ComponentTypeId type);
    
    virtual void createAndAddComponent(EntityId entity);
    
protected:
    EntityWorld* _entityWorld;
    ComponentTypeId _componentType;
};
