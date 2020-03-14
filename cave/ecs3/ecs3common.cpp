#include "pch.h"
#include <SDL2/SDL.h>

namespace ecs3
{
    static const int __sample = ComponentFactory::registerComponent<SampleComponent>();
    static const int __trans = ComponentFactory::registerComponent<TransformComponent>();

    ///////////////////////////////////////////////////
    //SampleSystem
    ///////////////////////////////////////////////////
    SampleSystem::SampleSystem(World* world)
        :System(world)
    {

    }

    void SampleSystem::onRegister()
    {
        addComponent<SampleComponent>();
    }

    void SampleSystem::onBeforeUpdate()
    {

    }

    void SampleSystem::onUpdate(BlockIterator& iterator)
    {
        //printf("Sample system update, size: %i\n", (int)iterator.size());
        Id* ids = iterator.getEntities();
        SampleComponent* components = iterator.getComponents<SampleComponent>();

        for (int i = 0; i < iterator.size(); i++)
        {
            Id id = ids[i];
            SampleComponent* comp = components + i;
            //printf("\tent: %u, test: %i, test2: %i\n", (unsigned int)id.index, comp->test, comp->test2);
            comp->test2++;
        }
    }

    ///////////////////////////////////////////////////
    //InputSystem
    ///////////////////////////////////////////////////
    InputSystem::InputSystem(World* world)
        :System(world)
    {

    }

    void InputSystem::onRegister()
    {

    }

    void InputSystem::onBeforeUpdate()
    {
        InputSingleton& input = _world->get<InputSingleton>();
        FrameSingleton& frame = _world->get<FrameSingleton>();

        if (input.keyStates[SDLK_w])
            input.forward = 1.0f;
        else if (input.keyStates[SDLK_s])
            input.forward = -1;
        else
            input.forward = 0;

        if (input.keyStates[SDLK_d])
            input.strafe = 1;
        else if (input.keyStates[SDLK_a])
            input.strafe = -1;
        else
            input.strafe = 0;

        input.cameraMove = input.mouse * input.mouseSpeed * frame.dt * 100.f;
        input.forward *= frame.dt;
        input.strafe *= frame.dt;
        input.mouse = glm::vec2(0.f, 0.f);
    }

    void InputSystem::onUpdate(BlockIterator& iterator)
    {
        printf("Input system update, size: %i\n", (int)iterator.size());
    }
    ///////////////////////////////////////////////////
    //Tests
    ///////////////////////////////////////////////////
    void TestIndex()
    {
        PackedArrayIndex<Id> index(10);
        for (int i = 0; i < 10; i++)
        {
            assert(index.create().isValid());
        }
    }

    void Test0()
    {
        World world;
        world.registerSystem<SampleSystem>();

        Configuration entConf;
        entConf.addComponent<SampleComponent>();
        Id ent = world.createEntity(entConf);
        printf("Created entity: %i\n", (int)ent.index);
        world.update();
        world.createEntity(entConf);
        world.update();
        world.createEntity(entConf);
        world.update();
        world.createEntity(entConf);
        world.update();
        world.createEntity(entConf);
        world.update();
        world.createEntity(entConf);
        world.update();
        world.createEntity(entConf);

        world.update();
        world.update();
        world.update();

    }

    void Tests()
    {
        TestIndex();
        Test0();
    }
}
