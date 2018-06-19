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
    Entity* _entity;
};

class Ecs;

struct Entity
{
    Entity(Ecs* ecs, Entity* parent)
        :_parent(parent)
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
        entity->_parent = this;
    }

    void removeChild(Entity* entity)
    {
        entity->_parent = nullptr;
        _children.erase(entity);
    }

    const glm::mat4& getGlobalTransform();

    void updateTransform()
    {
        _transformDirty = true;
        for (auto child : _children)
        {
            child->updateTransform();
        }
    }
};

class System
{
public:
    System() {}
    virtual ~System() {}
    
    virtual void beforeUpdate() {}
    virtual void update() {}
    virtual void afterUpdate() {}

    void init(Ecs* ecs);

    int _id;
    Ecs* _ecs;
};

struct RenderComponent : Component
{
    RenderComponent(int id, Entity* ent)
        :Component(id, ent)
    {

    }
};

class Renderer;

class RenderSystem : public System
{
public:
    RenderSystem(Renderer* renderer)
    {
        _renderer = renderer;
    }

    void addComponent(Entity* ent);

    virtual void afterUpdate();

    Renderer* _renderer;
};

class Ecs
{
public:
    Ecs();
    ~Ecs();

    std::unordered_set<Component*>& getComponets(int id);

    int addSystem(System* system);

public:
    void beforeUpdate();
    void update();
    void afterUpdate();

private:
    std::unordered_map<int, std::unordered_set<Component*>> _components;
    std::vector<System*> _systems;
};

