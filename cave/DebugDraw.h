#pragma once

#include "RenderElement.h"
#include "ShaderProgram.h"

const int c_maxDebugIndices = 1024;
class DebugDraw
{
public:
	DebugDraw();
	~DebugDraw();

	void addVec(const glm::vec3& a, const glm::vec3 b);
	void addLine(const glm::vec3& pos, const glm::vec3 vec);

	RenderElement m_element;
	IndexBuffer m_indices = IndexBuffer(c_maxDebugIndices);
	VertexBuffer m_vertices = VertexBuffer(c_maxDebugIndices, c_debugVf);
	ShaderProgram m_program;
	int m_currentIdx = 0;
	int m_currentVtx = 0;
};