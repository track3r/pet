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
    ,_curRotation(1024)
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
        const int transformPos = _entityWorld->getComponentPos(curEntId, TransformType);
        if (transformPos == -1)
        {
            continue;
        }

        //glm::mat4 inPos = *localPoses.getPointer(transformPos);
        const auto rotation = glm::mix(glm::quat(), _rotation.get(i), dt);
        glm::quat cur = _curRotation.get(i) * rotation;
        *_curRotation.getPointer(i) = cur;
        *localPoses.getPointer(transformPos) = glm::toMat4(cur);
    }
}

void AnimationSystem::removeComponent(ComponentId id)
{
    const int pos = _index.remove(id);
    _current.remove(pos);
    _rotation.remove(pos);
    _curRotation.remove(pos);
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
