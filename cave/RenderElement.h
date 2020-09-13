#pragma once

#include "VertexFormat.h"
#include "Texture.h"
#include <glm/glm.hpp>
#include "GlHeaders.h"
#include <vector>
#include "GpuBuffer.h"

class Texture;
class RenderContext;



class Buffer
{
public:
	const void* pointer();
	uint32_t memorySize();

	uint32_t elements()
	{
		return m_elements;
	}

protected:
	Buffer(size_t size, uint32_t elements, GLenum type)
		:m_buffer(size, 0xff)
		, m_type(type)
		, m_elements(elements)
	{}

	Buffer(Buffer&& other)
		:m_buffer(std::move(other.m_buffer))
		, m_type(other.m_type)
		, m_elements(other.m_elements)
	{
	}

	std::vector<unsigned char> m_buffer;

private:
	const GLenum m_type;
	const uint32_t m_elements;
};

class IndexBuffer : public Buffer
{
public:
	IndexBuffer(int32_t size)
		:Buffer(size * sizeof(uint32_t), size, GL_ELEMENT_ARRAY_BUFFER)
		, m_elementType(GL_UNSIGNED_INT)
	{}
	uint32_t* intPointer();
	GLenum type()
	{
		return m_elementType;
	}
private:
	GLenum m_elementType;
};

class VertexBuffer : public Buffer
{
public:
	VertexBuffer(uint32_t elements, const VertexFormat& format)
		:Buffer(format.size() * elements, elements, GL_ARRAY_BUFFER)
		, format(format)
	{}

	template<class T, VertexAttributeIndex a>
	T& value(uint32_t index)
	{
		assert(index < elements());
		void* base = &m_buffer[format.size() * index];
		return format.value<T, a>(base);
	}

	const VertexFormat& format;
};


class RenderElement
{
public:
	enum Flags
	{
		None = 0,
		NoTextures = 1,
	};

	RenderElement(GLenum primitive = GL_TRIANGLES);
	RenderElement(const RenderElement& other, int offset, int count);
	~RenderElement();

	void setupVbo(RenderContext* context, bool isStream);
	void setupEmptyVbo(RenderContext* context, bool isStream);
	void updateVbo(RenderContext* context);
	void render(RenderContext* context, uint8_t flags = None) const;

	IndexBuffer*	m_indices = nullptr;
	VertexBuffer*	m_vertices = nullptr;
	Texture*		textures[2] = { 0 };

	bool			_transparent = false;

    int				_offset = 0;
    int				_count = -1;
	

	//GLuint			m_objects[2];
	GpuBuffer		_vertexBuffer;
	GpuBuffer		_indexBuffer;
	GLuint			_vao = 0;
	bool			_isStream = false;
	GLenum			m_mode;
	bool			_reference = false;
};

