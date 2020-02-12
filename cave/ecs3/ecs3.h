#pragma once

#include "ecs3pch.h"

namespace ecs3
{
    class System;

    struct ComponentInfo
    {
        ComponentInfo()
            :id(-1)
            , size(0) 
        {}

        ComponentInfo(int id, size_t size)
            : id(id)
            , size(size) 
        {}

        int id;
        size_t size;
    };

    enum class ComponentType
    {
        Sample = 0,
        Max = 1,
    };

    class ComponentFactory
    {
    public:
        typedef std::function<void(uint8_t*)> CreateComponentFn;
        static void createComponent(int id, uint8_t* memory)
        {
            assert(_functions[id] != false);
            _functions[id](memory);
        }

        static void registerComponent(int id, CreateComponentFn function)
        {
            assert(_functions[id] == false);
            _functions[id] = function;
        }

        template<class T>
        static int registerComponent()
        {
            registerComponent(T::ID, [](uint8_t* memory) {new (memory) T(); });
            return T::ID;
        }
    private:
        static CreateComponentFn _functions[];
    };

    class Configuration
    {
    public:
        std::vector<ComponentInfo> _components;

        template<class C>
        Configuration& addComponent()
        {
            _components.emplace_back(ComponentInfo(C::ID, sizeof(C)));
            std::sort(begin(_components), end(_components), [](const ComponentInfo& a, const ComponentInfo b) {
                return a.id < b.id;
            });

            return *this;
        }

        bool operator==(const Configuration& other) const 
        {
            if (_components.size() != other._components.size())
            {
                return false;
            }

            for (int i = 0; i < _components.size(); i++)
            {
                if (_components[i].id != other._components[i].id)
                {
                    return false;
                }
            }
            return true;
        }

        bool matches(const Configuration& other)
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
                    i++;
                }
                else
                {
                    j++;
                }
            }

            return true;
        }
    };

    class Family
    {
    public:
        Family(Configuration configuration)
            :_configuration(configuration)
            ,_data((int)configuration._components.size())
            ,_index((int)configuration._components.size())
        {
            for (int i = 0; i < configuration._components.size(); i++)
            {
                _data[i].init(configuration._components[i].size);
            }
        }

        Id addEntity(int id)
        {
            Id localId = _index.create();
            _ids.add(id);
            for (int i = 0; i < _configuration._components.size(); i++)
            {
                uint8_t* dataPtr = _data[i].addPtr();
                ComponentFactory::createComponent(_configuration._components[i].id, dataPtr);
            }

            return localId;
        }

        void *getData(int id)
        {
            for (int i = 0; i < _configuration._components.size(); i++)
            {
                if (_configuration._components[i].id == id)
                {
                    return _data[i].getPointer(0);
                }
            }

            assert(false && "invalid id");
            return nullptr;
        }

        template<class T>
        T* getComponents()
        {
            return (T*) getData(T::ID);
        }

        int* getEntities()
        {
            return _ids.getPtr(0);
        }

        size_t size()
        {
            return _index.size();
        }

        Configuration           _configuration;
        std::vector<Data>       _data;
        PackedArrayIndex<Id>    _index;
        TypedData<int>          _ids;
    };

    //to be extended to handle non consecutive data blocks too
    struct BlockIterator
    {
        BlockIterator()
            :_family(nullptr)
            , _familyId(0)
        {

        }

        Family* _family;
        int _familyId;

        int* getEntities()
        {
            return _family->getEntities();
        }

        template<class T>
        T* getComponents()
        {
            return _family->getComponents<T>();
        }

        size_t size()
        {
            return _family->size();
        }
    };

    class Entity
    {
    public:
        Id          _localId;
        uint8_t     _family;
    };

    class World
    {
    public:
        template<typename T>
        void registerSystem() 
        {
            _systems.push_back(new T(this));
            _systems.back()->onRegister();
        }

        int createEntity(const Configuration& configuration)
        {
            _entities.emplace_back();
            int id = (int)_entities.size() - 1;
            for (int i = 0; i < _families.size(); i++)
            {
                if (_families[i]._configuration == configuration)
                {
                    _families[i].addEntity(id);
                    _entities[id]._family = i;
                    return id;
                }
            }

            _families.emplace_back(configuration);
            int familyId = (int)_families.size() - 1;
            _families[familyId].addEntity(id);
            _entities[id]._family = familyId;

            return id;
        }

        bool getBlock(const Configuration& configuration, BlockIterator& out)
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

        World()
        {

        }

        virtual ~World()
        {
            for (int i = 0; i < _systems.size(); i++)
            {
                //delete _systems[i];
            }
        }

        std::vector<System*>    _systems;
        std::vector<Family>     _families;
        std::vector<Entity>     _entities;
    };

    class Component
    {
    public:
        virtual ~Component() {}
    };

    class SampleComponent : public Component
    {
    public:
        SampleComponent()
        {
            void* ptr = this;
            int* ptr2 = (int*)&ptr;
            test = *ptr2;
        }
        int test;
        const static int ID = (int)ComponentType::Sample;
    };
    
    class System
    {
    public:
        System(World* world)
            :_world(world)
        {}

        template<class T>
        Configuration& addComponent()
        {
            _configuration.addComponent<T>();
        }

        bool update()
        {
            BlockIterator iterator;
            while (_world->getBlock(_configuration, iterator))
            {
                onUpdate(iterator);
            }
        }

        virtual ~System() {}
        virtual void onUpdate(BlockIterator& iterator) {}
        virtual void onRegister() {}

        int priority = 0;
    protected:
        World* _world;

    private:
        Configuration   _configuration;
    };

    class SampleSystem : public System
    {
        virtual void onRegister() override
        {
            addComponent<SampleComponent>();
            priority = 10000;
        }

        virtual void onUpdate(BlockIterator& iterator) override
        {
            
        }
    };
}
