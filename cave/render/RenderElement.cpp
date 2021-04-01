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

uint32_t* IndexData::intPointer()
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
	if (!_reference)
	{
		//glDeleteBuffers(2, m_objects);
	}

	//delete m_vertices;
	//delete m_indices;
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

RenderElement::RenderElement(const GpuBuffer& indexBuffer, uint32_t indexOffset, uint32_t count, const GpuBuffer& vertexBuffer, uint32_t vertexOffset, uint32_t vertexCount, const VertexFormat& format)
{
	_reference = true;
	_vertexBuffer.init(vertexBuffer, 0, 0);
	_indexBuffer.init(indexBuffer, 0, 0);
	_offset = indexOffset;
	_count = count;
	_vertexOffset = vertexOffset;
	
	m_vertices = new VertexData(vertexCount, format);
	m_indices = new IndexData(count);
}

void RenderElement::setupVbo(RenderContext* context, bool isStream)
{
	setupEmptyVbo(context, isStream);
	updateVbo(context);
}

void RenderElement::setupVao(GLuint vao, GpuBuffer& vertexBuffer, const VertexFormat& format)
{
	RenderContext::oglContext->bindVao(vao);
	RenderContext::oglContext->bindBuffer(vertexBuffer, GpuBuffer::Vertex);

	for (const auto& atr : c_attribs)
	{
		if (!format.haveAttribute(atr.index))
			continue;

		glEnableVertexAttribArray((int)atr.index);
		CheckGlError();

		glVertexAttribPointer((int)atr.index, atr.size, GL_FLOAT, GL_FALSE, (GLsizei)format.size(), (const void*)(size_t)format.attributeOffset(atr.index));
		CheckGlError();
	}
	RenderContext::oglContext->bindGlBuffer(GpuBuffer::Vertex, 0);
}

void RenderElement::setupEmptyVbo(RenderContext* context, bool isStream)
{
	if (_reference)
	{
		return;
	}
	_isStream = isStream;
	GLenum type = isStream ? GL_STREAM_DRAW : GL_STATIC_DRAW;
	
	_vertexBuffer.init(GpuBuffer::Vertex, m_vertices->memorySize());
	setupVao(_vao, _vertexBuffer, m_vertices->format);

	_indexBuffer.init(GpuBuffer::Index, m_indices->memorySize());
}

void RenderElement::updateVbo(RenderContext* context)
{
	if (m_vertices == nullptr || m_indices == nullptr)
	{
		return;
	}
	GLenum type = _isStream ? GL_STREAM_DRAW : GL_STATIC_DRAW;
	//LOG("[%i %i]", _offset * sizeof(uint32_t), _offset * sizeof(uint32_t) + m_indices->memorySize());
	_indexBuffer.update(_offset * sizeof(uint32_t), m_indices->memorySize(), m_indices->pointer());
	//RenderContext::oglContext->bindVao(_vao);
	_vertexBuffer.update(_vertexOffset * m_vertices->format.size(), m_vertices->memorySize(), m_vertices->pointer());
	//RenderContext::oglContext->bindVao(0);
	
}

void RenderElement::render(RenderContext* context, uint8_t flags) const
{
	context->bindVao(_vao);
	context->bindBuffer(_indexBuffer);
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