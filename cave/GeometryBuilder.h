#pragma once

#include "RenderElement.h"

class GeometryBuilder
{
public:
	GeometryBuilder(VertexBuffer& vertex, IndexBuffer& index);

	int addIndex(int idx);
	

private:
	int m_indexPos = 0;
	int m_vertexPos = 0;
	VertexBuffer& m_vertexBuffer;
	IndexBuffer& m_indexBuffer;
};

