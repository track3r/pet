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

        }
        void addEntity(int id)
        {
            //TODO implement
        }
        void *getData(int id, int pos)
        {

        }
        Configuration           _configuration;
        std::vector<Data>       _data;
        PackedArrayIndex<Id>    _index;
    };

    class Entity
    {
    public:
        Entity(Configuration configuration)
            :_configuration(configuration)
        {}
        Configuration   _configuration;
        int             _family;
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
            _entities.emplace_back(configuration);
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

    class System
    {
    public:
        System(World* world)
            :_world(world)
        {}
        virtual ~System() {}
        virtual void update() {}
        virtual void onRegister() {}

    protected:
        World*          _world;
        Configuration   _configuration;
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
        const static int ID = 0;
    };
    
    class SampleSystem : public System
    {
        virtual void onRegister()
        {
            _configuration.addComponent<SampleComponent>();
        }

        virtual void update() {}
    };
}
