#include "pch.h"
#include "TransformAnimationSystem.h"

static int __transformAnim = ecs3::ComponentFactory::registerComponent<TransformAnimationComponent>();

void AnimationDescription::addPoint(glm::vec3 pos)
{
    if (!_points.empty())
    {
        float distance = glm::distance(pos, _points.back());
        _distances.push_back(distance);
        _totalDistance += distance;

        _directions.push_back(glm::normalize(pos - _points.back()));
    }

    _points.push_back(pos);
}

TransformAnimationSystem::TransformAnimationSystem(ecs3::World* world)
    :System(world)
{

}

void TransformAnimationSystem::onRegister()
{
    addComponent<ecs3::TransformComponent>();
    addComponent<TransformAnimationComponent>();
}

void TransformAnimationSystem::onBeforeUpdate()
{

}

void TransformAnimationSystem::onUpdate(ecs3::BlockIterator& iterator)
{
    ecs3::InputSingleton& input = _world->get<ecs3::InputSingleton>();
    ecs3::FrameSingleton& frame = _world->get<ecs3::FrameSingleton>();

    for (int i = 0; i < iterator.size(); i++)
    {
        ecs3::Id& id = iterator.getEntities()[i];
        ecs3::TransformComponent& transform = iterator.getComponents<ecs3::TransformComponent>()[i];
        TransformAnimationComponent& anim = iterator.getComponents<TransformAnimationComponent>()[i];
        const AnimationDescription& descr = anim._description;
        const std::vector<float>& distances = descr.getDistances();
        if (distances.empty())
        {
            continue;
        }

        const std::vector<glm::vec3>& points = descr.getPoints();

        const float nextTime = anim._time + frame.dt;
        const float advance = descr._speed * frame.dt;
        float nextDistance = anim._distance + descr._speed * frame.dt;
        int keyIndex = 0;
        float rollingDistance = 0;
        float localDistance;

        const int last = (int)points.size() - 1;
        for (int i = 0; i < (int)points.size(); i++)
        {
            keyIndex = i;
            if (i == last)
            {
                nextDistance -= descr.getTotalDistance();
                localDistance = nextDistance;

                keyIndex = 0;
                break;
            }
            rollingDistance += distances[i];

            if (nextDistance < rollingDistance)
            {
                localDistance = nextDistance - (rollingDistance - distances[i]) ;
                break;
            }
        }

        glm::vec3 pos = points[keyIndex] + descr.getDirections()[keyIndex] * localDistance;

        anim._time += frame.dt;
        anim._distance = nextDistance;

        transform.matrix = glm::translate(glm::mat4(), pos);
    }
}