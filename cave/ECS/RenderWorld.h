#pragma once

#include "ComponentTypes.h"
#include "MeshSystem.h"
#include "../GlmIncludes.h"

class RenderWorld
{
public:
    MeshSystem _meshSystem;
    RenderWorld();
    DataWriter<glm::mat4, MeshId> getPositions();
    void update();
};
