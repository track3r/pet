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
"//precision mediump float; \n"
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


void DebugDraw::addLine(const glm::vec3& a, const glm::vec3 b, const glm::vec4 color, const glm::vec4 color2)
{
	if (m_currentIdx >= (c_maxDebugIndices - 2))
		return;

	m_vertices.value<glm::vec3, VertexAttributeIndex::Pos>(m_currentVtx) = a;
	m_vertices.value<glm::vec4, VertexAttributeIndex::Color>(m_currentVtx) = color;
	m_indices.intPointer()[m_currentIdx++] = m_currentVtx++;

	m_vertices.value<glm::vec3, VertexAttributeIndex::Pos>(m_currentVtx) = b;
	m_vertices.value<glm::vec4, VertexAttributeIndex::Color>(m_currentVtx) = color2;
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

void DebugDraw::reset()
{
	m_currentIdx = 0;
	m_currentVtx = 0;
}

void DebugDraw::drawCube(const glm::vec3& pos, const glm::vec4 color)
{
	float h = 0.5f;
	glm::vec3 p[] =
	{
		//tfl
		pos + glm::vec3(-h, h, h),
		//tfr
		pos + glm::vec3(h, h, h),
		//tnl
		pos + glm::vec3(-h, h, -h),
		//tnr
		pos + glm::vec3(h, h, -h),

		//bfl
		pos + glm::vec3(-h, -h, h),
		//bfr
		pos + glm::vec3(h, -h, h),
		//bnl
		pos + glm::vec3(-h, -h, -h),
		//bnr
		pos + glm::vec3(h, -h, -h)
	};

	addLine(p[0], p[1]);
	addLine(p[2], p[3]);
	addLine(p[0], p[2]);
	addLine(p[1], p[3]);

	addLine(p[0 + 4], p[1 + 4]);
	addLine(p[2 + 4], p[3 + 4]);
	addLine(p[0 + 4], p[2 + 4]);
	addLine(p[1 + 4], p[3 + 4]);

	addLine(p[0], p[0 + 4]);
	addLine(p[1], p[1 + 4]);
	addLine(p[2], p[2 + 4]);
	addLine(p[3], p[3 + 4]);
}

void DebugDraw::drawGrid()
{
	const int n = 200;
	for (int i = -n; i < n; i++)
	{
		addLine(glm::vec3(-n, 0, (float)i), glm::vec3(0, 0, (float)i), c_white, c_white);
		addLine(glm::vec3(0, 0, (float)i), glm::vec3(n, 0, (float)i), c_white, c_white);
	}

	for (int i = -n; i < n; i++)
	{
		addLine(glm::vec3((float)i, 0, -n), glm::vec3((float)i, 0, 0), c_white, c_white);
		addLine(glm::vec3((float)i, 0, -0), glm::vec3((float)i, 0, n), c_white, c_white);
	}

	addLine(glm::vec3(0, 0, 0), glm::vec3(n, 0, 0), c_red, c_red);
	addLine(glm::vec3(0, 0, 0), glm::vec3(0, n, 0), c_green, c_green);
	addLine(glm::vec3(0, 0, -0), glm::vec3(0, 0, n), c_blue, c_blue);

	//glm::mix(
	
}
