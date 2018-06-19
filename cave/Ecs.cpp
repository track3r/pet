#include "Ecs.h"
#include "Renderer.h"


Ecs::Ecs()
{
}


Ecs::~Ecs()
{
}

std::unordered_set<Component*>& Ecs::getComponets(int id)
{
    return _components[id];
}

int Ecs::addSystem(System * system)
{
    _systems.push_back(system);
    return (int)_systems.size() - 1;
}

void Ecs::beforeUpdate()
{
    for (auto& it : _systems)
    {
        it->beforeUpdate();
    }
}

void Ecs::update()
{
    for (auto& it : _systems)
    {
        it->update();
    }
}

void Ecs::afterUpdate()
{
    for (auto& it : _systems)
    {
        it->afterUpdate();
    }
}

void System::init(Ecs * ecs)
{
    _ecs = ecs;
    _id = ecs->addSystem(this);
}


void RenderSystem::addComponent(Entity * ent)
{
    auto& comps = _ecs->getComponets(_id);
    auto comp = new RenderComponent(_id, ent);
    comps.insert(comp);
}

void RenderSystem::afterUpdate()
{
    auto& comps = _ecs->getComponets(_id);
    for (auto& comp : comps)
    {
        const auto& mat = comp->_entity->getGlobalTransform();
        _renderer->renderCube(mat);
    }
}

inline const glm::mat4 & Entity::getGlobalTransform()
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
