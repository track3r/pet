#pragma once

#include "render/RenderElement.h"
#include "render/DebugDraw.h"

struct Mesh
{
	struct Vertex
	{
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec3 uv;
	};

	struct Triangle
	{
		int vertices[3] = { -1, -1, -1 };
		int neighbors[3] = { -1, -1, -1 };
	};

	std::vector<Vertex> vertices;
	std::vector<Triangle> triangles;

	//toBuffer(int offset, ...);
	//static Mesh fromBuffer(); ?
	//
};

class BufferBuilder
{
public:
	BufferBuilder(IndexData& index, VertexData& vertex, int indexPos = 0, int vertexPos = 0)
		: m_index(index)
		, m_vertex(vertex)
		, m_indexPos(indexPos)
		, m_vertexPos(vertexPos)
		, m_format(vertex.format)
	{

	}

	template <class T>
	void attribute(VertexAttributeIndex index, const T& value)
	{
        m_vertex.value<T, index>(m_vertexPos) = value;
	}

	void index(int idx)
	{
		m_index.intPointer()[m_indexPos++] = idx;
	}

	void nextVertex()
	{
		m_vertexPos++;
	}

	IndexData& m_index;
	VertexData& m_vertex;
	int m_indexPos = 0;
	int m_vertexPos = 0;
	const VertexFormat& m_format;
};

class Room
{
public:
	static void generate(DebugDraw& debug);
};

