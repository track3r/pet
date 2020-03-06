#pragma once

#include "RenderElement.h"
#include "ShaderProgram.h"

const int c_maxDebugIndices = 10240;

static const glm::vec4 c_red(1, 0, 0, 1);
static const glm::vec4 c_green(0, 1, 0, 1);
static const glm::vec4 c_blue(0, 0, 1, 1);
static const glm::vec4 c_white(1, 1, 1, 1);

class DebugDraw
{
public:
	DebugDraw();
	~DebugDraw();


	void addVec(const glm::vec3& a, const glm::vec3 b);
	void addLine(const glm::vec3& pos, const glm::vec3 vec, const glm::vec4 color = c_red, const glm::vec4 color2 = c_red);
	void drawGrid();
	void drawCube(const glm::vec3& pos, const glm::vec4 color = c_red);

	void reset();

	RenderElement m_element;
	IndexBuffer m_indices = IndexBuffer(c_maxDebugIndices);
	VertexBuffer m_vertices = VertexBuffer(c_maxDebugIndices * 2, c_debugVf);
	ShaderProgram m_program;
	int m_currentIdx = 0;
	int m_currentVtx = 0;
};