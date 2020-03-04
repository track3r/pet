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

    class SampleSystem final : public System
    {
    public:
        SampleSystem(World* world)
            :System(world)
        {

        }

        virtual void onRegister() override
        {
            addComponent<SampleComponent>();
        }

        virtual void onBeforeUpdate() override
        {

        }

        virtual void onUpdate(BlockIterator& iterator) override
        {
            printf("Sample system update, size: %i\n", (int)iterator.size());
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