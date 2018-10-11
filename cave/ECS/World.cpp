#include "pch.h"
#include "World.h"
#include "EntityWorld.h"
#include "RenderWorld.h"
#include "AnimationSystem.h"

World::World()
    :_entityWorld(new EntityWorld())
    ,_renderWorld(new RenderWorld())
{
    initSystems();
}

void World::initSystems()
{
    _entityWorld->_systems[MeshType] = &_renderWorld->_meshSystem;
    _entityWorld->_systems[AnimationType] = new AnimationSystem(_entityWorld);
    
}

void World::copyTransform()
{
    
}

void World::update(float dt)
{
    _entityWorld->update(dt);
    _entityWorld->_systems[AnimationType]->update(dt);
    copyTransform();
    _renderWorld->update(dt);
}

void World::render()
{
    _renderWorld->render();
}

void World::testEntities()
{
    
}

/*
EntityId createTestEntity(float posC, int idx)
{
    auto entId = world._entityWorld.create();
    auto ent = world._entityWorld.get(entId);
    auto renderCompId = world._entityWorld.addComponent(entId, MeshType);
    Vec4 pos = {0};
    pos.x = posC * 10 + idx * posC;
    world._renderWorld.getPositions().write(renderCompId, pos);
    return entId;
}

void debug()
{
    for (int i = 0; i < world._entityWorld._index._size; i++)
    {
        const auto entId = world._entityWorld._index.reverseLookup(i);
        const auto ent = world._entityWorld.get(entId);
        const auto pcCompId = ent->getComponent(ParentChildType);
        
        const auto meshCompId = ent->getComponent(MeshType);
        auto posIndex = world._renderWorld._meshSystem._index.lookup(meshCompId);
        auto pos = world._renderWorld._meshSystem._positions.getPointer(posIndex);
        
        bool hasHeight = pcCompId != invalid<ComponentId>();
        if (hasHeight)
        {
            const auto pcPos = world._entityWorld._parentChildSystem._index.lookup(pcCompId);
            const auto height = *world._entityWorld._parentChildSystem._heights.getPointer(pcPos);
            const auto sortDebug = *world._entityWorld._parentChildSystem._debugSort.getPointer(pcPos);
            int arrangement = world._entityWorld._parentChildSystem._index.lookup(pcCompId);
            printf("id: %i height: %i  %f pccid: %i sortDebug: %i pccPos: %i\n", (int)entId.index, height, pos->x, (int)pcCompId.index, sortDebug, arrangement);
        }
        else
        {
            printf("id: %i height: no  %f\n", (int)entId.index, pos->x);
        }
        
    }
}

void testParent()
{
    EntityId ids[9];
    EntityId ids3[2];
    EntityId ids2[5];
    for (int i = 0; i < 2; i++)
    {
        ids3[i] = createTestEntity(1.f, i);
    }
    
    for (int i = 0; i < 9; i++)
    {
        ids[i] = createTestEntity(1000.f, i);
    }
    
    for (int i = 0; i < 5; i++)
    {
        ids2[i] = createTestEntity(10.f, i);
        
    }
    debug();
    printf("parents\n");
    for (int i = 0; i < 2; i++)
    {
        world._entityWorld.setParent(ids3[i], ids2[i]);
    }
    
    for (int i = 0; i < 5; i++)
    {
        world._entityWorld.setParent(ids2[i], ids[i]);
    }
    
    debug();
    printf("sort\n");
    world._entityWorld._parentChildSystem.sortByParentChild();
    debug();
    
    printf("remove %i from parent\n", ids2[1].index);
    world._entityWorld.setParent(ids2[1], invalid<EntityId>());
    debug();
    
    printf("set %i parent to %i\n", ids2[1].index, ids2[0].index);
    world._entityWorld.setParent(ids2[1], ids2[0]);
    debug();
    
    printf("sort\n");
    world._entityWorld._parentChildSystem.sortByParentChild();
    debug();
}

void testReorder()
{
    int size = 7;
    int indirection[] = {1, 6, 4, 3, 2, 0, 5};
    
    int array[] = {0, 1, 2, 3, 4, 5, 6};
    int relocations[] = {0, 1, 2, 3, 4, 5, 6};
    
    for (int i = 0; i < size - 1; i++)
    {
        const int currentIndex = i;
        const int proper = indirection[i];
        const int properLocation = relocations[proper];
        const int toReloc = array[properLocation];
        const int relocTarget = array[currentIndex];
        const int a = currentIndex;
        const int b = properLocation;
        
        //printf("swap(%i, %i) proper=%i properLocation=%i toReloc=%i relocTarget=%i\n", a, b, proper, properLocation, toReloc, relocTarget);
        if (a == b)
        {
            //relocations[proper] = -1;
            continue;
        }
        
        relocations[relocTarget] = toReloc;
        relocations[currentIndex] = proper;
        relocations[proper] = -1;
        
        swap(array[a], array[b]);
    }
}
 */
