#pragma once

class ParentChildSystem : public System
{
public:
    PackedArrayIndex<ComponentId> _index;
    Data<EntityId> _parents;
    Data<EntityId> _current;
    //Data<EntityId> _groups;
    Data<int> _heights;
    Data<int> _debugSort;
    Data<Vec4> _localPos;
    Data<Vec4> _globalPos;
    bool _dirty = false;
    
    ParentChildSystem();
    
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
    
    void update() override;
    void removeComponent(ComponentId id) override;
    ComponentId createComponent() override;
    
    void modifyHeight(EntityId parentEnt, int amount);
    void setParent(ComponentId childComp, EntityId childEnt, ComponentId oldParentCompId, ComponentId newParentCompId, EntityId parentEntity);
    void sortByParentChild();
    void swapData(int a, int b);
};
