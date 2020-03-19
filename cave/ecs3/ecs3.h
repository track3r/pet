#pragma once

namespace ecs3
{
    class System;

    enum class ComponentType : int
    {
        Sample = 0,
        Transform = 1,
        Player = 2,
        Mesh = 3,
        Max,
    };

    enum class SingleComponentType
    {
        Frame,
        Input,
        Camera,
        Render,
        Max,
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

       

        bool haveComponent(int id);

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
        template<class C>
        Configuration& addComponent()
        {
            if (haveComponent(C::ID))
            {
                return *this;
            }
            int id = C::ID;
            _components.push_back(id);
            std::sort(begin(_components), end(_components));

            return *this;
        }
    };

    class PrefabData
    {
    public:

        template<class T>
        void addComponent(const T& component)
        {
            const uint16_t size = ComponentFactory::getComponentSize(T::ID);
            assert(_curOffset + size < 1024);
            if (_offsets[T::ID] == 0)
            {
                uint8_t* ptr = &_scratchMem[0] + _curOffset;
                ComponentFactory::copyComponent(T::ID, ptr, &component);
                _curOffset += size;
                _offsets[T::ID] = _curOffset;
            }
            else
            {
                uint8_t* ptr = &_scratchMem[0] + _offsets[T::ID] - size;
                ComponentFactory::copyComponent(T::ID, ptr, &component);
            }
        }

        const uint8_t* const getData(int id) const
        {
            assert(_offsets[id] != 0);
            const uint16_t size = ComponentFactory::getComponentSize(id);
            const int beginOffset = _offsets[id] - size;
            const uint8_t * const ptr = &_scratchMem[0] + beginOffset;
            return ptr;
        }

        uint16_t _offsets[(int)ComponentType::Max] = { 0 };
        uint8_t _scratchMem[1024];
        int _curOffset = 0;
    };

    class EntitityPrefab
    {
    public:
        Configuration _configuration;
        PrefabData _data;

        template<class T>
        void addComponent(const T& component)
        {
            _configuration.addComponent<T>();
            _data.addComponent(component);
        }
    };

    class Family
    {
    public:
        Family(Configuration configuration);

        Id addEntity(Id id);
        Id addEntity(Id id, const PrefabData& data);
        void* getData(int id);
        Id* getEntities();
        size_t size();

        template<class T>
        T* getComponents()
        {
            return (T*) getData(T::ID);
        }

        Configuration           _configuration;
        std::vector<Data>       _data;
        PackedArrayIndex<Id>    _index;
        TypedData<Id>          _externalIds;
    };

    //to be extended to handle non consecutive data blocks too
    struct BlockIterator
    {
        BlockIterator();

        Id* getEntities();
        size_t size();

        template<class T>
        T* getComponents()
        {
            return _family->getComponents<T>();
        }

        Family* _family;
        int _familyId;
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
        World();
        ~World();

        bool getBlock(const Configuration& configuration, BlockIterator& out);
        void update();
        Id createEntity(const Configuration& configuration, const PrefabData* data = nullptr);
        Id createEntity(const EntitityPrefab& prefab)
        {
            return createEntity(prefab._configuration, &prefab._data);
        }
        template<typename T>
        void registerSystem()
        {
            _systems.push_back(new T(this));
            _systems.back()->onRegister();
        }

        template<typename T>
        T& get()
        {
            if (_singletons[T::ID] == nullptr)
            {
                _singletons[T::ID] = (uint8_t*)new T();
            }

            return *((T*)_singletons[T::ID]);
        }
        
        std::vector<System*>    _systems;
        uint8_t*                _singletons[(int)SingleComponentType::Max] = { 0 };
        std::vector<Family>     _families;
        PackedArrayIndex<Id>    _index;
        TypedData<Entity>       _data;
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
        void beforeUpdate()
        {
            onBeforeUpdate();
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
        virtual void onBeforeUpdate() {}
        virtual void onRegister() {}

    protected:
        World* _world;
        Configuration   _configuration;
    };
}
