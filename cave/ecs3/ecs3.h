#pragma once

#include "pch.h"

namespace ecs3
{
    class System;

   

    struct ComponentInfo
    {
        int id;
        size_t size;
    };

    class RegisterContext
    {
    public:

    };

    class Data
    {
    public:
        void* _data = nullptr;
        int _size = 0;

    public:
        Data()
        {
        }

        void remove(int i)
        {
            assert(i > 0 && i < _data.size());
            if (i < _size - 1)
            {
                ::swap(_data[i], _data[_size - 1]);
            }

            _size--;
        }

        void add(const T& t)
        {
            _data[_size++] = t;
            //_data.push_back(t);
        }

        T* getPointer(int i)
        {
            assert(i >= 0 && i < _size);
            return &_data[i];
        }

        const T& get(int i) const
        {
            assert(i >= 0 && i < _size);
            return _data[i];
        }

        const T* getPointer(int i) const
        {
            assert(i >= 0 && i < _size);
            return &_data[i];
        }

        void swap(int a, int b)
        {
            assert(a >= 0 && a < _size);
            assert(b >= 0 && b < _size);
            ::swap(_data[a], _data[b]);
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
            _data[i].init(size, 1024);
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
