#include "ecs3.h"

namespace ecs3
{
   // int nextId = 0;
    //int SampleComponent::ID = nextId++;

    ComponentFactory::CreateComponentFn ComponentFactory::_functions[(size_t)ComponentType::Max];

    static const int __sample = ComponentFactory::registerComponent<SampleComponent>();
    static const int __trans = ComponentFactory::registerComponent<TransformComponent>();

    Id World::createEntity(const Configuration& configuration)
    {
        Id id = _index.create();

        Entity entity = Entity();
        for (int i = 0; i < _families.size(); i++)
        {
            if (_families[i]._configuration == configuration)
            {
                _families[i].addEntity(id);
                //_entities[id]._family = i;
                entity._family = i;
                _data.add(entity);
                return id;
            }
        }

        _families.emplace_back(configuration);
        int familyId = (int)_families.size() - 1;
        _families[familyId].addEntity(id);
        entity._family = familyId;
        _data.add(entity);
        return id;
    }

    bool World::getBlock(const Configuration& configuration, BlockIterator& out)
    {
        const int begin = out._family ? out._familyId + 1 : 0;
        for (int i = begin; i < _families.size(); i++)
        {
            if (_families[i]._configuration.matches(configuration))
            {
                out._family = &_families[i];
                out._familyId = i;
                return true;
            }
        }

        return false;
    }
    World::World()
        :_index(1024)
    {

    }
    
    void World::update()
    {
        for (System* sys : _systems)
        {
            sys->update();
        }
    }

    World::~World()
    {
        for (int i = 0; i < _systems.size(); i++)
        {
            //delete _systems[i];
        }
    }
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
    bool Configuration::matches(const Configuration& other)
    {
        int i = 0;
        int j = 0;

        while (i < other._components.size())
        {
            if (j >= _components.size())
            {
                return false;
            }

            int a = other._components[i].id;
            int b = _components[j].id;

            if (a == b)
            {
                i++;
                j++;
            }
            else if (a < b)
            {
                j++;
            }
            else
            {
                i++;
            }
        }

        return true;
    }
}