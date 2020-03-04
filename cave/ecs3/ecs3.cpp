#include "ecs3pch.h"

namespace ecs3
{
   // int nextId = 0;
    //int SampleComponent::ID = nextId++;

    ComponentFactory::CreateComponentFn ComponentFactory::_functions[(size_t)ComponentType::Max] = { 0 };
    uint16_t ComponentFactory::_sizes[(int)ComponentType::Max] = { 0 };

    Family::Family(Configuration configuration)
        :_configuration(configuration)
        , _data((int)configuration._components.size())
        , _index(1024)
    {
        for (int i = 0; i < configuration._components.size(); i++)
        {
            _data[i].init(ComponentFactory::getComponentSize(configuration._components[i]));
        }
    }

    Id Family::addEntity(Id id)
    {
        Id localId = _index.create();
        assert(localId.isValid());
        _externalIds.add(id);
        for (int i = 0; i < _configuration._components.size(); i++)
        {
            uint8_t* dataPtr = _data[i].addPtr();
            ComponentFactory::createComponent(_configuration._components[i], dataPtr);
            printf("%p\n", dataPtr);
        }

        return localId;
    }

    Id Family::addEntity(Id id, const PrefabData& data)
    {
        Id localId = _index.create();
        assert(localId.isValid());
        _externalIds.add(id);
        for (int i = 0; i < _configuration._components.size(); i++)
        {
            uint8_t* dataPtr = _data[i].addPtr();
            const int id = _configuration._components[i];
            ComponentFactory::copyComponent(id, dataPtr, data.getData(id));
            printf("%p\n", dataPtr);
        }

        return localId;
    }

    void* Family::getData(int id)
    {
        for (int i = 0; i < _configuration._components.size(); i++)
        {
            if (_configuration._components[i] == id)
            {
                return _data[i].getPointer(0);
            }
        }

        assert(false && "invalid id");
        return nullptr;
    }

    Id* Family::getEntities()
    {
        return _externalIds.getPtr(0);
    }

    size_t Family::size()
    {
        return _index.size();
    }

    Id World::createEntity(const Configuration& configuration, const PrefabData* data)
    {
        Id id = _index.create();

        Entity entity = Entity();
        for (int i = 0; i < _families.size(); i++)
        {
            if (_families[i]._configuration == configuration)
            {
                if (data == nullptr)
                {
                    _families[i].addEntity(id);
                }
                else
                {
                    _families[i].addEntity(id, *data);
                }
                
                //_entities[id]._family = i;
                entity._family = i;
                _data.add(entity);
                return id;
            }
        }

        _families.emplace_back(configuration);
        int familyId = (int)_families.size() - 1;
        if (data == nullptr)
        {
            _families[familyId].addEntity(id);
        }
        else
        {
            _families[familyId].addEntity(id, *data);
        }
        entity._family = familyId;
        _data.add(entity);
        return id;
    }

    bool World::getBlock(const Configuration& configuration, BlockIterator& out)
    {
        if (configuration._components.empty())
        {
            return false;
        }

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
            sys->onBeforeUpdate();
        }

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

    bool Configuration::matches(const Configuration& other)
    {
        int otherIndex = 0;
        int myIndex = 0;

        while (otherIndex < other._components.size())
        {
            if (myIndex >= _components.size())
            {
                return false;
            }

            int ot = other._components[otherIndex];
            int my = _components[myIndex];

            if (ot == my)
            {
                myIndex++;
                otherIndex++;
            }
            else if (ot < my)
            {
                return false;
            }
            else //other > my
            {
                myIndex++;
            }
        }

        return true;
    }
    
    BlockIterator::BlockIterator()
        :_family(nullptr)
        , _familyId(0)
    {

    }
    
    Id* BlockIterator::getEntities()
    {
        return _family->getEntities();
    }
    
    size_t BlockIterator::size()
    {
        return _family->size();
    }
}