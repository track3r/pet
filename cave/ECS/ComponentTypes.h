#pragma once

enum ComponentTypeId
{
    MeshType = 0,
    TransformType,
    AnimationType,
    MaxSystemsType,
};

static const char* g_ComponentNames[] = {
    "mesh", "parent", "animation", "invalid"
};

static const int c_maxComponents = MaxSystemsType;
