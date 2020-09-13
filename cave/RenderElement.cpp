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
	//m_objects[0] = other.m_objects[0];
	//m_objects[1] = other.m_objects[1];
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
	//glBindVertexArray(_vao);
	RenderContext::oglContext->bindVao(_vao);
	//glBindBuffer(GL_ARRAY_BUFFER, m_objects[0]);
	//CheckGlError();
	//glBufferData(GL_ARRAY_BUFFER, m_vertices->memorySize(), nullptr, type);
	//CheckGlError();
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

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_objects[1]);
	//CheckGlError();
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices->memorySize(), nullptr, type);
	//CheckGlError();
	_indexBuffer.init(GpuBuffer::Index, m_indices->memorySize());

	//glBindVertexArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//CheckGlError();
}

void RenderElement::updateVbo(RenderContext* context)
{
	//assert(_isStream);
	GLenum type = _isStream ? GL_STREAM_DRAW : GL_STATIC_DRAW;
	//glBindVertexArray(_vao);
	//glBindVertexArray(0); //ok too
	RenderContext::oglContext->bindVao(_vao);
	//glBindBuffer(GL_ARRAY_BUFFER, m_objects[0]);
	//CheckGlError();
	//glBufferData(GL_ARRAY_BUFFER, m_vertices->memorySize(), m_vertices->pointer(), type);
	//CheckGlError();
	_vertexBuffer.update(0, m_vertices->memorySize(), m_vertices->pointer());

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_objects[1]);
	//CheckGlError();
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices->memorySize(), m_indices->pointer(), type);
	//CheckGlError();
	_indexBuffer.update(0, m_indices->memorySize(), m_indices->pointer());

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void RenderElement::render(RenderContext* context, uint8_t flags) const
{
	context->bindVao(_vao);

	if ( (flags & NoTextures) == 0 )
	{
		if (textures[0] != nullptr)
		{
			//glBindTexture(GL_TEXTURE_2D, textures[0]->getTexture());
			context->bindTexture(*textures[0], 0);
		}		
	}

    const int count = _count != -1? _count: (int)m_indices->elements();
	glDrawElements(m_mode, count, m_indices->type(),(const void* )(_offset * sizeof(GLuint)));
	//glDrawArrays(m_mode, 0, count);
	CheckGlError();

	//glBindVertexArray(0);
}