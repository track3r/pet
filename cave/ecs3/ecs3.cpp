#include "ecs3.h"

namespace ecs3
{
   // int nextId = 0;
    //int SampleComponent::ID = nextId++;

    ComponentFactory::CreateComponentFn ComponentFactory::_functions[(size_t)ComponentType::Max];

    static const int __sample = ComponentFactory::registerComponent<SampleComponent>();
}