#pragma once

namespace ecs3
{
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