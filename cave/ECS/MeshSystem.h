#pragma once

#include "Ecs.h"
#include "../GlmIncludes.h"

typedef ComponentId MeshId;

class MeshSystem : public System
{
public:
    Data<glm::mat4> _positions;
    Data<uint16_t> _vbs;
    PackedArrayIndex<MeshId> _index;
    
    MeshSystem();
    
    void update() override;
    void removeComponent(ComponentId id) override;
    ComponentId createComponent() override;
    
    /*void debugComponent(ComponentId id, Writer* out) override
     {
     const int pos = _index.lookup(id);
     char tmp[32];
     auto vec = *_positions.getPointer(pos);
     sprintf(tmp, "{pos: (%f, %f, %f), vb: %i}\n", vec.x, vec.y, vec.z, (int) *_vbs.getPointer(pos));
     out->write(tmp);
     }*/
};
