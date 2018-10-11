#pragma once

class TransformSystem;

class AnimationSystem: public GameplaySystem
{
public:
    static const int type = ComponentTypeId::AnimationType;
    
    AnimationSystem(EntityWorld* world);
    
    void update(float dt) override;
    void removeComponent(ComponentId id) override;
    ComponentId createComponent(EntityId entity) override;
    
    ComponentId createComponent(EntityId entity, glm::quat rotation);
    
private:
    Data<glm::quat> _rotation;
    Data<glm::quat> _curRotation;
    TransformSystem* _transformSystem = nullptr;
};
