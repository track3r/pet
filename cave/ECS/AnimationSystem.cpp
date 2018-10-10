#include "pch.h"
#include "Ecs.h"

#include "ComponentTypes.h"
#include "EntityWorld.h"
#include "AnimationSystem.h"

#include "EntityWorld.h"

#include "TransformSystem.h"

AnimationSystem::AnimationSystem(EntityWorld* world)
    :GameplaySystem(world, AnimationType)
    ,_rotation(1024)
{
    
}

void AnimationSystem::update(float dt)
{
    if (_transformSystem == nullptr)
    {
        _transformSystem = _entityWorld->getSystem<TransformSystem>();
    }
    
     Data<glm::mat4>& localPoses = _transformSystem->_localPos;
    
    for(int i = 0; i < _index._size; i++)
    {
        const EntityId curEntId = *_current.getPointer(i);
        const ComponentId transformId = _entityWorld->getComponent(curEntId, TransformType);
        if (transformId == invalid<ComponentId>())
        {
            continue;
        }
        
        const pos = _en
        
        glm::mat4 inPos = *localPoses.getPointer()
    }
}

void AnimationSystem::removeComponent(ComponentId id)
{
    
}

ComponentId AnimationSystem::createComponent(EntityId entity)
{
    auto ret = System::createComponent(entity);
    _rotation.add(glm::quat());
    
    return ret;
}

ComponentId AnimationSystem::createComponent(EntityId entity, glm::quat rotation)
{
    auto ret = System::createComponent(entity);
    _rotation.add(rotation);
    
    return ret;
}
