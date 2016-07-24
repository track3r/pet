#include "DebugDraw.h"

const char c_vShaderStr[] =
"attribute vec4 v_position; \n"
"attribute vec4 v_color; \n"
"uniform mat4 vp_matrix; \n"
"varying vec4 var_color;\n"
"void main() \n"
"{ \n"
" var_color = v_color;\n"
" gl_Position = vp_matrix * v_position; \n"
"} \n";

const char c_fShaderStr[] =
"precision mediump float; \n"
"varying vec4 var_color;"
"void main() \n"
"{ \n"
" gl_FragColor = var_color; \n"
"} \n";

DebugDraw::DebugDraw()
	:m_element(GL_LINES)
	,m_program(c_vShaderStr, c_fShaderStr)
{
	m_element.m_indices = &m_indices;
	m_element.m_vertices = &m_vertices;
}


DebugDraw::~DebugDraw()
{
}

static const glm::vec4 c_red(1, 0, 0, 1);
static const glm::vec4 c_green(0, 1, 0, 1);
static const glm::vec4 c_blue(0, 0, 1, 1);
static const glm::vec4 c_white(1, 1, 1, 1);

void DebugDraw::addLine(const glm::vec3& a, const glm::vec3 b)
{
	if (m_currentIdx >= (c_maxDebugIndices - 2))
		return;

	m_vertices.value<glm::vec3, VertexAttributeIndex::Pos>(m_currentVtx) = a;
	m_vertices.value<glm::vec4, VertexAttributeIndex::Color>(m_currentVtx) = c_red;
	m_indices.intPointer()[m_currentIdx++] = m_currentVtx++;

	m_vertices.value<glm::vec3, VertexAttributeIndex::Pos>(m_currentVtx) = b;
	m_vertices.value<glm::vec4, VertexAttributeIndex::Color>(m_currentVtx) = c_red;
	m_indices.intPointer()[m_currentIdx++] = m_currentVtx++;
}

void DebugDraw::addVec(const glm::vec3& pos, const glm::vec3 vec)
{
	if (m_currentIdx >= (c_maxDebugIndices - 6))
		return;

	int idx = 0;

	for (int i = 1; i <= 4; i++)
	{
		m_indices.intPointer()[m_currentIdx + idx++] = m_currentVtx;
		m_indices.intPointer()[m_currentIdx + idx++] = m_currentVtx + i;
	}

	m_currentIdx += idx;

	glm::vec3 positions[] =
	{
		pos,
		pos + glm::vec3(vec.x, 0, 0),
		pos + glm::vec3(0, vec.y, 0),
		pos + glm::vec3(0, 0, vec.z),
		pos + vec
	};

	glm::vec4 colors[] =
	{
		c_white, c_red, c_green, c_blue, c_white
	};

	for (int i = 0; i < 5; i++)
	{
		m_vertices.value<glm::vec3, VertexAttributeIndex::Pos>(m_currentVtx) = positions[i];
		m_vertices.value<glm::vec4, VertexAttributeIndex::Color>(m_currentVtx) = colors[i];
		m_currentVtx++;
	}
}