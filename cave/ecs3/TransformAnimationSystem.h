#pragma once

class AnimationDescription
{
public:
    AnimationDescription()
    {

    }
    AnimationDescription(float speed)
        :_speed(speed)
    {

    }
    void addPoint(glm::vec3 pos);

    const std::vector<glm::vec3>& getPoints() const
    {
        return _points;
    }

    const std::vector<float>& getDistances() const
    {
        return _distances;
    }

    const std::vector<glm::vec3>& getDirections() const
    {
        return _directions;
    }

    float getTotalDistance() const
    {
        return _totalDistance;
    }

    float _speed = 1;
    bool _mirrorRepeat = false;
protected:
    std::vector<glm::vec3> _points;
    std::vector<glm::vec3> _directions;
    std::vector<float> _distances;
    float _totalDistance = 0;    
};

struct TransformAnimationComponent
{
    TransformAnimationComponent()
    {
    }

    TransformAnimationComponent(const AnimationDescription& description)
        :_description(description)
    {

    }
    AnimationDescription _description;
    float _time = 0;
    float _distance = 0;
    bool _reverse = false;
    
    static const int ID = (int)ecs3::ComponentType::TransformAnim;
};

class TransformAnimationSystem final : public ecs3::System
{
public:
    TransformAnimationSystem(ecs3::World* world);

    virtual void onRegister() override;
    virtual void onBeforeUpdate() override;
    virtual void onUpdate(ecs3::BlockIterator& iterator) override;
};