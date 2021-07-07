#include "pch.h"
#include "GeometryBuilder.h"

GeometryBuilder::GeometryBuilder()
{
    _indexData = new IndexData(1024);
    _vertexData = new VertexData(2048, c_defaultVf);
}

GeometryBuilder::~GeometryBuilder()
{
    delete _indexData;
    delete _vertexData;
}

int GeometryBuilder::point(glm::vec3 pos)
{
    _points.emplace_back(pos);
    return (int)_points.size() - 1;
}

bool GeometryBuilder::isReverseQuad(const glm::vec3 points[4], glm::vec3 normal)
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

void GeometryBuilder::box(glm::vec3 center, float side)
{
    const float halfSide = side / 2;
    //top
    float yTop = center.y + halfSide;
    int topPoints[] = {
        point(glm::vec3(center.x - halfSide, yTop, center.z + halfSide)),
        point(glm::vec3(center.x + halfSide, yTop, center.z + halfSide)),
        point(glm::vec3(center.x + halfSide, yTop, center.z - halfSide)),
        point(glm::vec3(center.x - halfSide, yTop, center.z - halfSide)),
    };

    glm::vec3 topNormal(0.f, 1.f, 0.f);
    quad(topPoints, topNormal);

    //bottom
    float yBottom = center.y - halfSide;
    int bottomPoints[] = {
        point(glm::vec3(center.x - halfSide, yBottom, center.z + halfSide)),
        point(glm::vec3(center.x + halfSide, yBottom, center.z + halfSide)),
        point(glm::vec3(center.x + halfSide, yBottom, center.z - halfSide)),
        point(glm::vec3(center.x - halfSide, yBottom, center.z - halfSide)),
    };

    glm::vec3 bottomNormal(0.f, -1.f, 0.f);
    quad(bottomPoints, bottomNormal);

    //front
    float zFront = center.z - halfSide;
    int frontPoints[] = {
        point(glm::vec3(center.x - halfSide, center.y - halfSide, zFront)),
        point(glm::vec3(center.x + halfSide, center.y - halfSide, zFront)),
        point(glm::vec3(center.x + halfSide, center.y + halfSide, zFront)),
        point(glm::vec3(center.x - halfSide, center.y + halfSide, zFront)),
    };

    glm::vec3 frontNormal(0.f, 0.f, -1.f);
    quad(frontPoints, frontNormal);

    //back
    float zBack = center.z + halfSide;
    int backPoints[] = {
        point(glm::vec3(center.x - halfSide, center.y - halfSide, zBack)),
        point(glm::vec3(center.x + halfSide, center.y - halfSide, zBack)),
        point(glm::vec3(center.x + halfSide, center.y + halfSide, zBack)),
        point(glm::vec3(center.x - halfSide, center.y + halfSide, zBack)),
    };

    glm::vec3 backNormal(0.f, 0.f, 1.f);
    quad(backPoints, backNormal);

    //left
    float xLeft = center.x + halfSide;
    int leftPoints[] = {
        point(glm::vec3(xLeft, center.y - halfSide, center.z + halfSide)),
        point(glm::vec3(xLeft, center.y - halfSide, center.z - halfSide)),
        point(glm::vec3(xLeft, center.y + halfSide, center.z - halfSide)),
        point(glm::vec3(xLeft, center.y + halfSide, center.z + halfSide)),
    };

    glm::vec3 leftNormal(1.f, 0.f, 0.f);
    quad(leftPoints, leftNormal);

    //right
    float xRight = center.x - halfSide;
    int rightPoints[] = {
        point(glm::vec3(xRight, center.y - halfSide, center.z + halfSide)),
        point(glm::vec3(xRight, center.y - halfSide, center.z - halfSide)),
        point(glm::vec3(xRight, center.y + halfSide, center.z - halfSide)),
        point(glm::vec3(xRight, center.y + halfSide, center.z + halfSide)),
    };

    glm::vec3 rightNormal(-1.f, 0.f, 0.f);
    quad(rightPoints, rightNormal);
}

void GeometryBuilder::quad(int points[4], glm::vec3 normal)
{
    glm::vec3 pointsPos[] = { _points[points[0]], _points[points[1]] , _points[points[2]] , _points[points[3]] };
    glm::vec2 uvs[] = { glm::vec2(0.f,0.f) , glm::vec2(1.f, 0.f), glm::vec2(1.f, 1.f), glm::vec2(0.f, 1.f) };
    const bool isRevese = isReverseQuad(pointsPos, normal);

    int indices[4] = {
        addVertex(pointsPos[0], uvs[0], normal),
        addVertex(pointsPos[1], uvs[1], normal),
        addVertex(pointsPos[2], uvs[2], normal),
        addVertex(pointsPos[3], uvs[3], normal),
    };

    int indices1[3] = { points[0], points[2], points[3] };
    triangle(indices1, normal);

    int indices2[3] = { points[0], points[1], points[2] };
    triangle(indices2, normal);
}

void GeometryBuilder::triangle(int indices[3], glm::vec3 normal)
{
    const glm::vec3 points[] =
    {
        _vertexData->value<glm::vec3, VertexAttributeIndex::Pos>(indices[0]),
        _vertexData->value<glm::vec3, VertexAttributeIndex::Pos>(indices[1]),
        _vertexData->value<glm::vec3, VertexAttributeIndex::Pos>(indices[2]),
    };

    const glm::vec3 a = points[1] - points[0];
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

void GeometryBuilder::addIndex(uint32_t index)
{
    _indexData->intPointer()[_indexPos++] = index;
}

int GeometryBuilder::addVertex(glm::vec3 pos, glm::vec2 uv, glm::vec3 normal)
{
    _vertexData->value<glm::vec3, VertexAttributeIndex::Pos>(_vertexPos) = pos;
    _vertexData->value<glm::vec2, VertexAttributeIndex::Uv>(_vertexPos) = uv;
    _vertexData->value<glm::vec3, VertexAttributeIndex::Normal>(_vertexPos) = normal;
    return _vertexPos++;
}

Geometry GeometryBuilder::getGeometry()
{
    Geometry output = Geometry(_indexPos, _vertexPos, _vertexData->format);
    memcpy(output.indexData.pointer(), _indexData->pointer(), _indexPos * sizeof(uint32_t));
    memcpy(output.vertexData.pointer(), _vertexData->pointer(), _vertexPos * _vertexData->format.size());
    return output;
}