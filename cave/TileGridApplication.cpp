#include "pch.h"

#include "ecs3/SampleRenderSystem.h"
#include "ecs3/PlayerSystem.h"
#include "ecs3/MeshRenderSystem.h"
#include "ecs3/TransformAnimationSystem.h"
#include "TileGridApplication.h"
#include "render/RenderWorld.h"
#include "ObjReader.h"
#include "Job.h"
#include "Utils.h"

void TileGridApplication::subInit()
{
    RenderWorld* renderWorld = _world->get<RenderSingleton>().world;

}
