#pragma once

#include "Ecs.h"
#include "../GlmIncludes.h"

typedef ComponentId MeshId;

class MeshSystem : public System
{
public:
    static const int type = ComponentTypeId::MeshType;
    
    Data<glm::mat4> _positions;
    Data<uint16_t> _vbs;
    
    MeshSystem();
    
    void update(float dt) override;
    void removeComponent(ComponentId id) override;
    ComponentId createComponent(EntityId entityId) override;
    
    /*void debugComponent(ComponentId id, Writer* out) override
     {
     const int pos = _index.lookup(id);
     char tmp[32];
     auto vec = *_positions.getPointer(pos);
     sprintf(tmp, "{pos: (%f, %f, %f), vb: %i}\n", vec.x, vec.y, vec.z, (int) *_vbs.getPointer(pos));
     out->write(tmp);
     }*/
};
