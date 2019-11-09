#pragma once

#include "pch.h"

namespace ecs3
{
    class System;

    struct ComponentInfo
    {
        ComponentInfo(int id, size_t size)
            : id(id)
            , size(size) {}
        int id;
        size_t size;
    };

    class RegisterContext
    {
    public:
        std::vector<ComponentInfo> _components;
        template<class C>
        void addComponent()
        {
            _components.(ComponentInfo(C::ID, sizeof(C)));
        }
    };

    class Family
    {
    public:
        Family(int componetsNumber)
            :_components(componetsNumber)
            ,_data(componetsNumber)
        {

        }
        void initComponent(int i, int id, size_t size)
        {
            _components[i].id = id;
            _components[i].size = size;
            _data[i].init(size);
        }

        void *getData(int id, int pos)
        {

        }
        std::vector<ComponentInfo> _components;
        std::vector<Data> _data;
    };

    class World
    {
    public:
        template<typename T>
        void RegisterSystem() 
        {
            _systems.push_back(new T());
            RegisterContext context;
            _systems.back()->onRegister(context);
        }

        World()
        {

        }

        virtual ~World()
        {
            for (int i = 0; i < _systems.size(); i++)
            {
                delete _systems[i];
            }
        }

        std::vector<System*> _systems;
        std::vector<Family> _families;
    };

    class System
    {
    public:
        System(World* world)
            :_world(world)
        {}
        virtual ~System() {}
        virtual void update() {}
        virtual void onRegister(RegisterContext& context) {}

    private:
        World* _world;
    };

    //need size, id
    class Component
    {
    public:
        virtual ~Component() {}
    };
    
    class SampleSystem : public System
    {
        virtual void onRegister(RegisterContext& context)
        {

        }

        virtual void update() {}
    };
}
