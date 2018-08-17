#pragma once

enum ComponentTypeId
{
    MeshType = 0,
    ParentChildType,
    MaxSystemsType,
};

static const char* g_ComponentNames[] = {
    "mesh", "parent", "invalid"
};

static const int c_maxComponents = MaxSystemsType;
