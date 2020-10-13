#include "pch.h"
#include "GeometryBuilder.h"


GeometryBuilder::GeometryBuilder(VertexData& vertex, IndexData& index)
	:m_vertexBuffer(vertex)
	, m_indexBuffer(index)
{
}
