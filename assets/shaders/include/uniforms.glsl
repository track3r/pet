#include "uniformStructs.h"

layout(std140, binding = 1) uniform View
{
    ViewMatrices viewMatrices;
    ivec4 shadowAtlasParams;
};

layout(std140, binding = 2) uniform Model
{
    mat4 modelMatrix;
};
