#include "ecs3pch.h"

namespace ecs3
{
    static const int __sample = ComponentFactory::registerComponent<SampleComponent>();
    static const int __trans = ComponentFactory::registerComponent<TransformComponent>();

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