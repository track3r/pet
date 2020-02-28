#pragma once

#include "ecs3pch.h"

//TODO:
//+entity ids
//+world update
//+sample system test
//+transform component
//+cube component + system
//+integrtate
//+test multi family
//create component params
//remove entity
//singletons
//player controller, camera
//sorta mesh component
//mesh loader
//lighting system
//shadows
//textures
//voxel chunk component

//landscape or dungeon gen
//collision
//ai agents
//weapons

namespace ecs3
{
    class System;

    enum class ComponentType
    {
        Sample = 0,
        Transform = 1,
        Max = 2,
    };

    class ComponentFactory
    {
    public:
        enum class Operation
        {
            New, Move, Copy, Delete
        };
        typedef void(*CreateComponentFn)(uint8_t*, void* param, const void* constParam, Operation op);
        static void createComponent(int id, uint8_t* memory)
        {
            assert(_functions[id] != nullptr);
            _functions[id](memory, nullptr, nullptr, Operation::New);
        }
        
        static void moveComponent(int id, uint8_t* memory, void* existing)
        {
            assert(_functions[id] != nullptr);
            _functions[id](memory, existing, nullptr, Operation::Move);
        }

        static void copyComponent(int id, uint8_t* memory, const void* existing)
        {
            assert(_functions[id] != nullptr);
            _functions[id](memory, nullptr, existing, Operation::Copy);
        }

        static void deleteComponent(int id, uint8_t* memory)
        {
            assert(_functions[id] != nullptr);
            _functions[id](memory, nullptr, nullptr, Operation::Delete);
        }

        static void registerComponent(int id, CreateComponentFn function, uint16_t size)
        {
            assert(_functions[id] == nullptr);
            _functions[id] = function;
            _sizes[id] = size;
        }

        static uint16_t getComponentSize(int id)
        {
            return _sizes[id];
        }

        template<class T>
        static int registerComponent()
        {
            CreateComponentFn funcP = &componentNew<T>;
            registerComponent(T::ID, funcP, (uint16_t) sizeof(T));
            return T::ID;
        }

    private:
        template<class T>
        static void componentNew(uint8_t* memory, void* param, const void* constParam, Operation op)
        {
            T* prev = (T*)param;
            const T* prevConst = (T*)constParam;
            switch (op)
            {
            case ecs3::ComponentFactory::Operation::New:
                new (memory) T();
                break;
            case ecs3::ComponentFactory::Operation::Move:
                new (memory) T(std::move(*prev));
                break;
            case ecs3::ComponentFactory::Operation::Copy:
                new (memory) T((*prevConst));
                break;
            case ecs3::ComponentFactory::Operation::Delete:
                T* obj = (T*)memory;
                obj->~T();
                break;
            }
        }
        static CreateComponentFn _functions[];
        static uint16_t _sizes[];
    };

    
    class Configuration
    {
    public:
        std::vector<int> _components;

        template<class C>
        Configuration& addComponent()
        {
            _components.emplace_back(C::ID);
            std::sort(begin(_components), end(_components));

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
                if (_components[i] != other._components[i])
                {
                    return false;
                }
            }
            return true;
        }

        bool matches(const Configuration& other);
    };

    class EntitityPrefab
    {
    public:
        Configuration _configuration;

        template<class T>
        void addComponent(T component)
        {
            const uint16_t size = ComponentFactory::getComponentSize(T::ID);
            assert(_curOffset + size < 1024);
            _configuration.addComponent<T>();
            uint8_t* ptr = &_scratchMem[0] + _curOffset;
            ComponentFactory::copyComponent(T::ID, ptr, &component);
            _curOffset += size;
            _offsets[T::ID] = _curOffset;
        }

        const uint8_t* getData(int id)
        {
            assert(_offsets[id] != 0);
            const uint16_t size = ComponentFactory::getComponentSize(id);
            const int beginOffset = _offsets[id] - size;
            uint8_t* ptr = &_scratchMem[0] + beginOffset;
            return ptr;
        }

        uint16_t _offsets[(int)ComponentType::Max] = { 0 };
        uint8_t _scratchMem[1024];
        int _curOffset = 0;
    };

    class Family
    {
    public:
        Family(Configuration configuration)
            :_configuration(configuration)
            ,_data((int)configuration._components.size())
            ,_index(1024)
        {
            for (int i = 0; i < configuration._components.size(); i++)
            {
                _data[i].init( ComponentFactory::getComponentSize(configuration._components[i]));
            }
        }

        Id addEntity(Id id)
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

        void *getData(int id)
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

        template<class T>
        T* getComponents()
        {
            return (T*) getData(T::ID);
        }

        Id* getEntities()
        {
            return _externalIds.getPtr(0);
        }

        size_t size()
        {
            return _index.size();
        }

        Configuration           _configuration;
        std::vector<Data>       _data;
        PackedArrayIndex<Id>    _index;
        TypedData<Id>          _externalIds;
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

        Id* getEntities()
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

        Id createEntity(const Configuration& configuration);
        bool getBlock(const Configuration& configuration, BlockIterator& out);
        World();
        void update();
        virtual ~World();

        std::vector<System*>    _systems;
        std::vector<Family>     _families;
        PackedArrayIndex<Id> _index;
        TypedData<Entity> _data;
    };

    class SampleComponent
    {
    public:
        SampleComponent()
            :test2(0)
        {
            void* ptr = this;
            int* ptr2 = (int*)&ptr;
            test = *ptr2;
        }
        int test;
        int test2;
        const static int ID = (int)ComponentType::Sample;
    };

    class TransformComponent
    {
    public:
        TransformComponent()
            :matrix()
        {
        }

        TransformComponent(glm::vec3 origin)
            :matrix(glm::translate(glm::mat4(), origin))
        {

        }

        glm::mat4 matrix;
        const static int ID = (int)ComponentType::Transform;
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
            return _configuration;
        }

        bool update()
        {
            BlockIterator iterator;
            while (_world->getBlock(_configuration, iterator))
            {
                onUpdate(iterator);
            }

            return true;
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
    public:
        SampleSystem(World* world)
            :System(world)
        {

        }
        virtual void onRegister() override
        {
            addComponent<SampleComponent>();
            priority = 10000;
        }

        virtual void onUpdate(BlockIterator& iterator) override
        {
            printf("Sample system update, size: %i\n", (int) iterator.size());
            Id* ids = iterator.getEntities();
            SampleComponent* components = iterator.getComponents<SampleComponent>();

            for (int i = 0; i < iterator.size(); i++)
            {
                Id id = ids[i];
                SampleComponent* comp = components + i;
                printf("\tent: %u, test: %i, test2: %i\n", (unsigned int)id.index, comp->test, comp->test2);
                comp->test2++;
            }
        }
    };

    void Test0();
    void Tests();
}
