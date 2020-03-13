#include "ecs3pch.h"
#include "MeshRenderSystem.h"

#include "../Application.h"

static int __mesh = ecs3::ComponentFactory::registerComponent<MeshComponent>();

MeshComponent::~MeshComponent()
{
}

MeshRenderSystem::MeshRenderSystem(ecs3::World* world)
    :System(world)
{
}

void MeshRenderSystem::onRegister()
{
    addComponent<ecs3::TransformComponent>();
    addComponent<MeshComponent>();
}

void MeshRenderSystem::onBeforeUpdate()
{
}

void MeshRenderSystem::onUpdate(ecs3::BlockIterator& iterator)
{
    Renderer* renderer = Application::getRenderer();

    ecs3::InputSingleton& input = _world->get<ecs3::InputSingleton>();
    ecs3::FrameSingleton& frame = _world->get<ecs3::FrameSingleton>();

    for (int i = 0; i < iterator.size(); i++)
    {
        ecs3::Id& id = iterator.getEntities()[i];
        ecs3::TransformComponent& transform = iterator.getComponents<ecs3::TransformComponent>()[i];
        MeshComponent& mesh = iterator.getComponents<MeshComponent>()[i];
        
        if (mesh.mesh != nullptr)
        {
            renderer->renderElement(*renderer->getDefaultProgram(), *mesh.mesh, transform.matrix);
        }
    }
}
