#include "pch.h"
#include "TileGrid.h"

struct GridInfo_t
{
    int width;
    int height;
};

TileGridSystem::TileGridSystem(ecs3::World* world)
    :ecs3::System(world)
{
}

void TileGridSystem::onRegister()
{
    addComponent<ecs3::TransformComponent>();
    addComponent<TileGridComponent>();
}

void TileGridSystem::onBeforeUpdate()
{
}

void TileGridSystem::onUpdate(ecs3::BlockIterator& iterator)
{

}

void TileGridSystem::onCreateEntity(ecs3::Id entId, ecs3::EntityAccessor& entity)
{
}

void TileGridSystem::onDeleteEntity(ecs3::Id entId, ecs3::EntityAccessor& entity)
{
}

class MeshGen
{
public:
    struct Vert
    {
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 uv;
    };

    struct Triangle
    {
        int verts[3];
        glm::vec3 normal;
    };

    struct Quad
    {
        int points[4];
        glm::vec3 normal;
    };

    int point(glm::vec3 pos)
    {
        _points.emplace_back(pos);
        return _points.size() - 1;
    }

    bool isReverseQuad(const glm::vec3 points[4], glm::vec3 normal)
    {
        glm::vec3 a = points[1] - points[0];
        glm::vec3 b = points[3] - points[0];
        glm::vec3 c = glm::cross(a, b);
        if (glm::dot(c, normal) >= 0)
        {
            return false;
        }

        return true;
    }

    void quad(int points[4], glm::vec3 normal)
    {
        glm::vec3 pointsPos[] = { _points[points[0]], _points[points[1]] , _points[points[2]] , _points[points[3]] };
        const bool isRevese = isReverseQuad(pointsPos, normal);

        int indices[4] = {
            addVertex(points)
        };
        int points1[3] = { points[0], points[2], points[3] };
        glm::vec2 uv1[3] = { glm::vec2(0.f,0.f), glm::vec2(1.f, 1.f), glm::vec2(0.f, 1.f)};
        triangle(points1, uv1, normal);

        int points2[3] = { points[0], points[1], points[2] };
        glm::vec2 uv2[3] = { glm::vec2(0.f,0.f), glm::vec2(1.f, 0.f), glm::vec2(1.f, 1.f) };
        triangle(points2, uv2, normal);
    }

    void triangle(glm::vec3 points[3], glm::vec3 normal, int indices[3])
    {
        glm::vec3 a = points[1] - points[0];
        glm::vec3 b = points[2] - points[0];
        glm::vec3 c = glm::cross(a, b);
        if (glm::dot(c, normal) >= 0)
        {
            addIndex(indices[0]);
            addIndex(indices[1]);
            addIndex(indices[2]);
        }
        else
        {
            addIndex(indices[2]);
            addIndex(indices[1]);
            addIndex(indices[0]);
        }
    }
    void addIndex(uint32_t index)
    {
        _indexData->intPointer()[_indexPos++] = index;
    }

    int addVertex(glm::vec3 pos, glm::vec2 uv, glm::vec3 normal)
    {
        _vertexData->value<glm::vec3, VertexAttributeIndex::Pos>(_vertexPos) = pos;
        _vertexData->value<glm::vec2, VertexAttributeIndex::Uv>(_vertexPos) = uv;
        _vertexData->value<glm::vec3, VertexAttributeIndex::Normal>(_vertexPos) = normal;
        return _vertexPos++;
    }

    MeshGen()
    {
        _indexData = new IndexData(1024);
        _vertexData = new VertexData(2048, c_defaultVf);
    }
public:
    std::vector<Vert> _vertices;
    std::vector<glm::vec3> _points;
    IndexData* _indexData;
    int _indexPos = 0;

    VertexData* _vertexData;
    int _vertexPos = 0;
};