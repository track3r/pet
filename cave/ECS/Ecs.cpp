#include "pch.h"

#include "Ecs.h"

System::System()
    :_current(1024)
    ,_index(1024)
{
    
}

ComponentId System::createComponent(EntityId enitity)
{
    _current.add(enitity);
    return _index.create();
}
