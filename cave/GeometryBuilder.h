#pragma once

#include "render/RenderElement.h"

class GeometryBuilder
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

    GeometryBuilder();
    ~GeometryBuilder();

    int point(glm::vec3 pos);
    bool isReverseQuad(const glm::vec3 points[4], glm::vec3 normal);
    void box(glm::vec3 center, float side);
    void quad(int points[4], glm::vec3 normal);
    Geometry getGeometry();
private:
    void triangle(int indices[3], glm::vec3 normal);
    void addIndex(uint32_t index);
    int addVertex(glm::vec3 pos, glm::vec2 uv, glm::vec3 normal);

public:
    std::vector<Vert> _vertices;
    std::vector<glm::vec3> _points;
    IndexData* _indexData;
    int _indexPos = 0;

    VertexData* _vertexData;
    int _vertexPos = 0;
};

