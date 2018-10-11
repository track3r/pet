#include "pch.h"
#include "Ecs.h"

#include "ComponentTypes.h"
#include "TransformSystem.h"

#include "EntityWorld.h"



TransformSystem::TransformSystem()
:_parents(1024)
,_heights(1024)
,_localPos(1024)
,_globalPos(1024)
,_debugSort(1024)
{
}

/*void debugComponent(ComponentId id, Writer* out) override
 {
 if (id == invalid<ComponentId>())
 {
 out->write("{invalid}");
 return;
 }
 
 int pos = _index.lookup(id);
 assert(pos != -1);
 
 char tmp[256];
 
 sprintf(tmp, "{}");
 out->write("{");
 {
 out->write("parent: ");
 sprintf(tmp,"%i", (int)_parents.getPointer(pos)->index);
 out->write(tmp);
 
 out->write(", current: ");
 sprintf(tmp,"%i", (int)_current.getPointer(pos)->index);
 out->write(tmp);
 
 out->write(", h: ");
 sprintf(tmp,"%i", *_heights.getPointer(pos));
 out->write(tmp);
 
 out->write(", dbg: ");
 sprintf(tmp,"%i", *_debugSort.getPointer(pos));
 out->write(tmp);
 
 }
 out->write("}");
 
 
 }*/

void TransformSystem::update(float dt)
{
    if (_dirty)
    {
        sortByParentChild();
        _dirty = false;
    }
}

void TransformSystem::removeComponent(ComponentId id)
{
    int dataId = _index.swapToEndAndRemove(id);
    _parents.remove(dataId);
    _heights.remove(dataId);
    _localPos.remove(dataId);
    _globalPos.remove(dataId);
    _current.remove(dataId);
}

ComponentId TransformSystem::createComponent(EntityId entityId)
{
    ComponentId ret = System::createComponent(entityId);
    
    glm::mat4 zero;
    _localPos.add(zero);
    _globalPos.add(zero);
    
    _heights.add(0);
    _parents.add(EntityId(-1, -1));
    _debugSort.add(ret.index);
    
    return ret;
}

void TransformSystem::modifyHeight(EntityId parentEnt, int amount)
{
    for (int i = 0; i < _index._size; i++)
    {
        if (*_parents.getPointer(i) == parentEnt)
        {
            *_heights.getPointer(i) += amount;
        }
    }
    
    for (int i = 0; i < _index._size; i++)
    {
        if (*_parents.getPointer(i) == parentEnt)
        {
            const ComponentId childId = _index.reverseLookup(i);
            const EntityId parentEntId = *_current.getPointer(childId.index);
            modifyHeight(parentEntId, amount);
        }
    }
}

void TransformSystem::setParent(ComponentId childComp, ComponentId oldParentCompId, ComponentId newParentCompId, EntityId parentEntity)
{
    const int childPos = _index.lookup(childComp);
    
    EntityId childEnt = *_current.getPointer(childPos);
    
    int oldParentHeight = 0;
    if (oldParentCompId != invalid<ComponentId>())
    {
        int oldParentPos = _index.lookup(oldParentCompId);
        oldParentHeight = *_heights.getPointer(oldParentPos);
    }
    
    *_parents.getPointer(childPos) = parentEntity;
    
    if (parentEntity != invalid<EntityId>())
    {
        int change = 1 - oldParentHeight;
        if (newParentCompId != invalid<ComponentId>())
        {
            const int parentPos = _index.lookup(newParentCompId);
            const int parentHeight = *_heights.getPointer(parentPos);
            change += parentHeight;
            
            const glm::mat4 parentGlobalTransform = *_globalPos.getPointer(parentPos);
            const glm::mat4 childGlobalTransform = *_globalPos.getPointer(childPos);
            //const Vec4 childLocalTransform = parentGlobalTransform.transpose() * childGlobalTransform
        }
        else
        {
            //no parent
            //const Vec4 childLocalTransform =  childGlobalTransform
        }
        
        *_heights.getPointer(childPos) += change;
        modifyHeight(childEnt, change);
        
    }
    else
    {
        const int change = -oldParentHeight - 1;
        *_heights.getPointer(childPos) += change;
        modifyHeight(childEnt, change);
    }
    
    _dirty = true;
}

template <class T>
struct DataComp
{
    const Data<T>& data;
    DataComp(const DataComp& other):
    data(other.data)
    {
    }
    
    DataComp(const Data<T>& data):
    data(data)
    {}
    
    bool operator()(int a, int b)
    {
        return !(*data.getPointer(a) < *data.getPointer(b));
    }
};

void TransformSystem::sortByParentChild()
{
    short* indirection = (short*) alloca(sizeof(short) * _index._size);
    short* relocations = (short*) alloca(sizeof(short) * _index._size);
    short* array = (short*) alloca(sizeof(short) * _index._size);
    
    for (int i = 0; i < _index._size; i++)
    {
        indirection[i] = i;
        relocations[i] = i;
        array[i] = i;
    }
    
    std::sort(indirection, indirection + _index._size, DataComp<int>(_heights));
    
    /*for (int j = 0; j < _index._size; j++)
     {
     printf("%i ", indirection[j]);
     }
     printf("\n");*/
    
    for (int i = 0; i < _index._size - 1; i++)
    {
        const int currentIndex = i;
        //i = 0, proper = 1, properLocation = 1, toReloc = 0 = i relocTarget=properLocation=1
        const int proper = indirection[i];
        const int properLocation = relocations[proper];
        const int toReloc = array[properLocation];
        const int relocTarget = array[currentIndex];
        const int a = currentIndex;
        const int b = properLocation;
        
        //printf("swap(%i, %i) proper=%i properLocation=%i toReloc=%i relocTarget=%i\n", a, b, proper, properLocation, toReloc, relocTarget);
        if (a == b)
        {
            relocations[proper] = -1;
            /*for (int j = 0; j < _index._size; j++)
             {
             printf("%i ", relocations[j]);
             }
             printf("\n");*/
            continue;
        }
        
        
        relocations[relocTarget] = toReloc;
        relocations[currentIndex] = proper;
        relocations[proper] = -1;
        /*for (int j = 0; j < _index._size; j++)
         {
         printf("%i ", relocations[j]);
         }
         printf("\n");*/
        
        swapData(a, b);
        swap(array[a], array[b]);
    }
    
    _dirty = false;
}

void TransformSystem::updateTransforms()
{
    for (int i = 0; i < _index._size; i++)
    {
        const auto parent = *_parents.getPointer(i);
        //const auto parentGlobalTransform = ..
        //const auto childLocalTransform
        //childGlobal = parentGlobalTransform * childLocalTransform;
        //childGlobal = childLocalTransform
    }
}

void TransformSystem::swapData(int a, int b)
{
    if (a == b)
    {
        return;
    }
    
    _index.swapData(a, b);
    _localPos.swap(a, b);
    _heights.swap(a, b);
    _parents.swap(a, b);
    _current.swap(a, b);
    _debugSort.swap(a, b);
    //debug();
}
