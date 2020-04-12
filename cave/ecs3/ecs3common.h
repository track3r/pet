#pragma once

namespace ecs3
{
    template<class T, int Id>
    class AutoRegisterComponent
    {
    public:
        static const int ID;
    };

    template<class T, int Id>
    const int AutoRegisterComponent<T, Id>::ID(ComponentFactory::registerComponent<T>(Id));

    class SampleComponent : public AutoRegisterComponent<SampleComponent, (int)ComponentType::Sample>
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
    };

    class TransformComponent : public AutoRegisterComponent<TransformComponent, (int)ComponentType::Transform>
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
    };

    struct FrameSingleton
    {
        float dt = 0;
        uint64_t number = 0;

        static const int ID = -1;
    };

    struct InputSingleton
    {
        void keyDown(uint32_t key)
        {
            keyStates[key] = 1;
        }

        void keyUp(uint32_t key)
        {
            keyStates[key] = 0;
        }

        float forward = 0;
        float strafe = 0;

        float mouseSpeed;
        glm::vec2 mouse;
        glm::vec2 cameraMove;
        std::unordered_map<uint32_t, char>  keyStates;

        static const int ID = -1;
    };


    class SampleSystem final : public System
    {
    public:
        SampleSystem(World* world);
        virtual void onRegister() override;
        virtual void onBeforeUpdate() override;
        virtual void onUpdate(BlockIterator& iterator) override;
    };

    class InputSystem final : public System
    {
    public:
        InputSystem(World* world);
        virtual void onRegister() override;
        virtual void onBeforeUpdate() override;
        virtual void onUpdate(BlockIterator& iterator) override;
    };

    void Test0();
    void Tests();
}