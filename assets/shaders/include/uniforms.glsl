struct ViewMatrices
{
    mat4 view;
    mat4 projection;
};

layout(std140, binding = 1) uniform View
{
    ViewMatrices viewMatrices;
};

layout(std140, binding = 2) uniform Model
{
    mat4 modelMatrix;
};

struct LightParams
{
    mat4 matrix;
    vec4 pos;
    vec4 atlas;
};

layout(std140, binding = 3) uniform Light
{
    LightParams lightParams[100];
    int numLights;
};


struct InstanceData
{
    mat4 tranform;
};

layout(std140, binding = 4) uniform Instance
{
    InstanceData instance[1000];
};