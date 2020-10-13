#pragma once

#include "RenderElement.h"

class GeometryBuilder
{
public:
	GeometryBuilder(VertexData& vertex, IndexData& index);

	int addIndex(int idx);
	

private:
	int m_indexPos = 0;
	int m_vertexPos = 0;
	VertexData& m_vertexBuffer;
	IndexData& m_indexBuffer;
};

