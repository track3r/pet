#include "pch.h"
#include "RenderElement.h"
#include "Renderer.h"

const void* Buffer::pointer()
{
	return &m_buffer[0];
}

uint32_t Buffer::memorySize()
{
	return (uint32_t)m_buffer.size();
}

uint32_t* IndexBuffer::intPointer()
{
	return (uint32_t*)pointer();
}

RenderElement::RenderElement(GLenum primitive)
	:m_mode(primitive)
{
	glGenVertexArrays(1, &_vao);
	textures[0] = nullptr;
	textures[1] = nullptr;
}

RenderElement::~RenderElement()
{
	//TODO: fixme
	//glDeleteBuffers(2, m_objects);
}

RenderElement::RenderElement(const RenderElement& other, int offset, int count)
{
	_reference = true;
	_vertexBuffer.init(other._vertexBuffer, 0, 0);
	_indexBuffer.init(other._indexBuffer, 0, 0);
	_offset = offset;
	_count = count;
	m_vertices = other.m_vertices;
	m_indices = other.m_indices;
	_vao = other._vao;
	m_mode = other.m_mode;
}

void RenderElement::setupVbo(RenderContext* context, bool isStream)
{
	setupEmptyVbo(context, isStream);
	updateVbo(context);
}

void RenderElement::setupEmptyVbo(RenderContext* context, bool isStream)
{
	_isStream = isStream;
	GLenum type = isStream ? GL_STREAM_DRAW : GL_STATIC_DRAW;
	RenderContext::oglContext->bindVao(_vao);
	_vertexBuffer.init(GpuBuffer::Vertex, m_vertices->memorySize());

	const VertexFormat& format = m_vertices->format;
	for (const auto& atr : c_attribs)
	{
		if (!format.haveAttribute(atr.index))
			continue;

		glEnableVertexAttribArray((int)atr.index);
		CheckGlError();
		glVertexAttribPointer((int)atr.index, atr.size, GL_FLOAT, GL_FALSE, (GLsizei)format.size(), (const void*)(size_t)format.attributeOffset(atr.index));
		CheckGlError();
	}

	_indexBuffer.init(GpuBuffer::Index, m_indices->memorySize());
}

void RenderElement::updateVbo(RenderContext* context)
{
	GLenum type = _isStream ? GL_STREAM_DRAW : GL_STATIC_DRAW;
	RenderContext::oglContext->bindVao(_vao);
	_vertexBuffer.update(0, m_vertices->memorySize(), m_vertices->pointer());
	_indexBuffer.update(0, m_indices->memorySize(), m_indices->pointer());
}

void RenderElement::render(RenderContext* context, uint8_t flags) const
{
	context->bindVao(_vao);

	if ( (flags & NoTextures) == 0 )
	{
		if (textures[0] != nullptr)
		{
			context->bindTexture(*textures[0], 0);
		}		
	}

    const int count = _count != -1? _count: (int)m_indices->elements();
	glDrawElements(m_mode, count, m_indices->type(),(const void* )(_offset * sizeof(GLuint)));
	CheckGlError();
}