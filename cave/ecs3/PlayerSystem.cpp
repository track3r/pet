#include "pch.h"
#include "PlayerSystem.h"
#include "../Application.h"

static int __player = ecs3::ComponentFactory::registerComponent<PlayerComponent>();

PlayerSystem::PlayerSystem(ecs3::World* world)
    :System(world)
{
}

void PlayerSystem::onRegister()
{
    addComponent<PlayerComponent>();
    addComponent<ecs3::TransformComponent>();
}

void PlayerSystem::onBeforeUpdate()
{
}

void PlayerSystem::onUpdate(ecs3::BlockIterator& iterator)
{
    assert(iterator.size() == 1); //one player for now
    ecs3::Id& id = *iterator.getEntities();

    ecs3::InputSingleton& input = _world->get<ecs3::InputSingleton>();
    ecs3::FrameSingleton& frame = _world->get<ecs3::FrameSingleton>();
    ecs3::TransformComponent& transform = *iterator.getComponents<ecs3::TransformComponent>();

    transform.matrix = glm::rotate(transform.matrix, input.cameraMove.x, glm::vec3(0.f, 1.f, 0.f));
    transform.matrix = glm::rotate(transform.matrix, input.cameraMove.y, glm::vec3(1.f, 0.f, 0.f));

    //printf("%f\n", input.forward);
    transform.matrix = glm::translate(transform.matrix, glm::vec3(input.strafe * 10, 0.f, -input.forward * 10));

    
    Application::getRenderer()->camera().setView(glm::inverse(transform.matrix));
}
