#pragma once

namespace ecs3
{
    class SampleRenderSystem : public System
    {
    public:
        SampleRenderSystem(World* world);
        virtual void onRegister() override;
        virtual void onUpdate(BlockIterator& iterator) override;
    };
}

