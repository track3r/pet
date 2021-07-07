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
#include "GeometryBuilder.h"

bool createCubeMesh(RenderContext& context, RenderGeometryManager& geometryManager, RenderElement& result)
{
    GeometryBuilder builder;
    builder.box(glm::vec3(), 1.f);
    Geometry boxGeo = builder.getGeometry();
    bool ret = geometryManager.allocate(result, boxGeo.indexData.elements(), boxGeo.vertexData.elements());
    if (!ret)
    {
        LOG("geometry allocation failed");
        return false;
    }

    result.updateVbo(&context, boxGeo);

    return true;
}

RenderElement boxMeshElem;
Texture defaultTexture;

void initResources(RenderWorld* renderWorld, RenderContext& context)
{
    RenderGeometryManager& geometryManager = renderWorld->getGeometryManager();
    createCubeMesh(context, geometryManager, boxMeshElem);
    defaultTexture.init("..\\assets\\sponza\\textures\\spnza_bricks_a_diff.png");
    boxMeshElem.textures[0] = &defaultTexture;
}

void TileGridApplication::subInit()
{
    RenderWorld* renderWorld = _world->get<RenderSingleton>().world;    
    RenderGeometryManager& geometryManager = renderWorld->getGeometryManager();

    initResources(renderWorld, _renderer.getRenderContext());    

    MeshComponent meshComp;
    ecs3::EntitityPrefab meshConf;
    meshConf.addComponent(ecs3::TransformComponent(glm::vec3(0.f)));
    meshComp.mesh = _world->get<RenderSingleton>().world->createMesh(boxMeshElem);
    meshConf.addComponent(meshComp);
    ecs3::Id id = _world->createEntity(meshConf);

    ecs3::Id lightId = renderWorld->createLight(RenderLight());
    ecs3::EntitityPrefab lightConf;
    lightConf.addComponent(ecs3::TransformComponent(glm::vec3(0.f, 10.f, 0.f)));
    lightConf.addComponent(LightComponent(lightId));
    _world->createEntity(lightConf);
}

void TileGridApplication::subUpdate()
{
}
