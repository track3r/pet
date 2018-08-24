#include "pch.h"
#include "MeshSystem.h"

MeshSystem::MeshSystem()
:_index(1024)
,_positions(1024)
,_vbs(1024)
{
    
}

void MeshSystem::update()
{
    for(int i = 0; i < _index._size; i++)
    {
        glm::mat4* pos = _positions.getPointer(i);
        int vb = *_vbs.getPointer(i);
        //render(pos, vb)
    }
}

void MeshSystem::removeComponent(ComponentId id)
{
    int dataId = _index.swapToEndAndRemove(id);
    _positions.remove(dataId);
    _vbs.remove(dataId);
}

ComponentId MeshSystem::createComponent()
{
    MeshId ret = _index.create();
    
    glm::mat4 zero;
    _positions.add(zero);
    
    _vbs.add(0);
    return ret;
}

/*void debugComponent(ComponentId id, Writer* out) override
 {
 const int pos = _index.lookup(id);
 char tmp[32];
 auto vec = *_positions.getPointer(pos);
 sprintf(tmp, "{pos: (%f, %f, %f), vb: %i}\n", vec.x, vec.y, vec.z, (int) *_vbs.getPointer(pos));
 out->write(tmp);
 }*/
