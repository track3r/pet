#pragma once

#include <glm\glm.hpp>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>

struct Entity;

struct Component
{
    virtual ~Component() 
    {}

    Component(int type, Entity* entity)
        : _type(type)
        , _entity(entity)
    {}

    const int _type;
    Entity const* _entity;
};

class Ecs;

struct Entity
{
    Entity(Ecs* ecs, Entity* parent)
    {
        if(parent != nullptr)
            parent->addChild(this);
    }

    glm::mat4 _transform;
    glm::mat4 _globalTransform;
    bool _transformDirty = false;

    Entity* _parent;
    std::unordered_set<Entity*> _children;
    std::vector<Component*> _components;

    void addChild(Entity* entity)
    {
        _children.insert(entity);
        entity->_parent = entity;
    }

    void removeChild(Entity* entity)
    {
        entity->_parent = nullptr;
        _children.erase(entity);
    }

    const glm::mat4 getGlobalTransform()
    {
        if (!_transformDirty)
        {
            return _globalTransform;
        }

        glm::mat4 parent = glm::mat4();

        if (_parent != nullptr)
        {
            parent = _parent->getGlobalTransform();
        }

        _globalTransform = parent * _transform;
        _transformDirty = false;

        return _globalTransform;
    }

    void updateTransform()
    {
        _transformDirty = true;
    }
};

class System
{
public:
    System() {}
    virtual ~System() {}
    
    virtual void beforeUpdate(Ecs* ecs) {}
    virtual void update(Ecs* ecs) {}
    virtual void afterUpdate(Ecs* ecs) {}
};

class Ecs
{
public:
    Ecs();
    ~Ecs();

    std::vector<Component*>& getComponets(int id)
    {
        return _components[id];
    }

public:
private:
    virtual void beforeUpdate(Ecs* ecs) {}
    virtual void update(Ecs* ecs) {}
    virtual void afterUpdate(Ecs* ecs) {}

private:
    std::unordered_map<int, std::vector<Component*>> _components;
    std::vector<System*> _systems;
};

