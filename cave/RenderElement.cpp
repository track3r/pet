#include "pch.h"
#include "RenderElement.h"

const void* Buffer::pointer()
{
	return &m_buffer[0];
}

size_t Buffer::memorySize()
{
	return m_buffer.size();
}

uint32_t* IndexBuffer::intPointer()
{
	return (uint32_t*)pointer();
}

RenderElement::RenderElement(GLenum primitive)
	:m_mode(primitive)
{
	glGenBuffers(2, m_objects);
	glGenVertexArrays(1, &_vao);
	textures[0] = nullptr;
	textures[1] = nullptr;
}

RenderElement::~RenderElement()
{
	//TODO: fixme
	//glDeleteBuffers(2, m_objects);
}

void RenderElement::setupVbo(bool isStream)
{
	_isStream = isStream;
	GLenum type = isStream ? GL_STREAM_DRAW : GL_STATIC_DRAW;
	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_objects[0]);
	CheckGlError();
	glBufferData(GL_ARRAY_BUFFER, m_vertices->memorySize(), m_vertices->pointer(), type);
	CheckGlError();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_objects[1]);
	CheckGlError();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices->memorySize(), m_indices->pointer(), type);
	CheckGlError();

	const auto& format = m_vertices->format;
	for (const auto& atr : c_attribs)
	{
		if (!format.haveAttribute(atr.index))
			continue;

		glEnableVertexAttribArray((int)atr.index);
		CheckGlError();
		glVertexAttribPointer((int)atr.index, atr.size, GL_FLOAT, GL_FALSE, (GLsizei)format.size(), (const void*)(size_t)format.attributeOffset(atr.index));
		CheckGlError();
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	CheckGlError();
	
}

void RenderElement::updateVbo()
{
	assert(_isStream);
	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_objects[0]);
	//CheckGlError();
	glBufferData(GL_ARRAY_BUFFER, m_vertices->memorySize(), m_vertices->pointer(), GL_STREAM_DRAW);
	CheckGlError();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_objects[1]);
	//CheckGlError();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices->memorySize(), m_indices->pointer(), GL_STREAM_DRAW);
	CheckGlError();

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void RenderElement::render(uint8_t flags) const
{
	glBindVertexArray(_vao);

	if ( (flags & NoTextures) == 0 )
	{
		if (textures[0] != nullptr)
		{
			glBindTexture(GL_TEXTURE_2D, textures[0]->getTexture());
		}		
	}

    const int count = _count != -1? _count: (int)m_indices->elements();
	glDrawElements(m_mode, count, m_indices->type(), NULL);
	//glDrawArrays(m_mode, 0, count);
	CheckGlError();

	//glBindVertexArray(0);
}