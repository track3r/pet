#include "pch.h"
#include "MeshRenderSystem.h"

#include "../Application.h"
#include "../RenderWorld.h"

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
        
        if (mesh.mesh != ecs3::invalid<ecs3::Id>())
        {
            //renderer->renderElement(*renderer->getDefaultProgram(), *mesh.mesh, transform.matrix);
            _world->get<RenderSingleton>().world->transform(mesh.mesh, transform.matrix);
        }
    }
}


LightComponent::~LightComponent()
{
}

LightRenderSystem::LightRenderSystem(ecs3::World* world)
    :System(world)
{

}

void LightRenderSystem::onRegister()
{
    addComponent<ecs3::TransformComponent>();
    addComponent<LightComponent>();
}

void LightRenderSystem::onBeforeUpdate()
{

}

void LightRenderSystem::onUpdate(ecs3::BlockIterator& iterator)
{
    Renderer* renderer = Application::getRenderer();

    ecs3::InputSingleton& input = _world->get<ecs3::InputSingleton>();
    ecs3::FrameSingleton& frame = _world->get<ecs3::FrameSingleton>();
    RenderWorld* render = _world->get<RenderSingleton>().world;
    for (int i = 0; i < iterator.size(); i++)
    {
        ecs3::Id& id = iterator.getEntities()[i];
        ecs3::TransformComponent& transform = iterator.getComponents<ecs3::TransformComponent>()[i];
        LightComponent& light = iterator.getComponents<LightComponent>()[i];

        if (light.light != ecs3::invalid<ecs3::Id>())
        {
            //renderer->renderElement(*renderer->getDefaultProgram(), *mesh.mesh, transform.matrix);
            //_world->get<RenderSingleton>().world->updateLightPos(light.light, glm::vec3(transform.matrix[3]));
            RenderLight lightParams;
            lightParams.pos = glm::vec3(transform.matrix[3]);
            //light.direction = glm::vec3(glm::vec4(1, 0, 0, 0) * transform.matrix);
            lightParams.direction = glm::vec3(1, 0, 0);
            render->updateLight(light.light, lightParams);
        }
    }
}