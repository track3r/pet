#include "pch.h"
#include "GpuBuffer.h"
#include "Renderer.h"

GpuBuffer::~GpuBuffer()
{

}

bool GpuBuffer::init(Type type, uint32_t size, const char* debugName)
{
	assert(_apiObject == 0);
	_type = type;
	_size = size;
	glGenBuffers(1, &_apiObject);
	RenderContext* ogl = RenderContext::oglContext;
	ogl->bindGlBuffer(type, _apiObject);
	if (debugName != nullptr)
	{
		glObjectLabel(GL_BUFFER, _apiObject, -1, debugName);
	}
	glBufferData(RenderContext::glBufferType(type), size, NULL, GL_STATIC_DRAW);

	
	CheckGlError();
	
	return true;
}

bool GpuBuffer::init(const GpuBuffer& ref, uint32_t offset, uint32_t size)
{
	_isReference = true;
	_offset = offset + ref._offset;
	if (size == 0)
	{
		size = ref._size;
	}
	_size = size;
	_apiObject = ref._apiObject;
	_type = ref._type;
	_mappedUpdate = ref._mappedUpdate;
	return true;
}

void GpuBuffer::update(uint32_t offset, uint32_t size, const void* data) const
{
	
	RenderContext* ogl = RenderContext::oglContext;
	ogl->bindGlBuffer(_type, _apiObject);
	//glBindBuffer(RenderContext::glBufferType(_type), _apiObject);
	if (_mappedUpdate) {
		char* ptr = nullptr;
		glGetBufferPointerv(RenderContext::glBufferType(_type), GL_BUFFER_MAP_POINTER, &((void*)ptr));
		if (ptr != nullptr)
		{
			LOG("buffer mapped!");
			glUnmapBuffer(RenderContext::glBufferType(_type));
		}
		
		ptr = (char*)glMapBuffer(RenderContext::glBufferType(_type), GL_WRITE_ONLY);
		memcpy(ptr + offset + _offset, data, size);
		glUnmapBuffer(RenderContext::glBufferType(_type));
		return;
	}

	if (offset == 0 && _offset == 0 && size == _size)
	{
		glBufferData(RenderContext::glBufferType(_type), size, data, GL_STATIC_DRAW);
		CheckGlError();
	}
	else
	{
		glBufferSubData(RenderContext::glBufferType(_type), offset + _offset, size, data);
		CheckGlError();
	}
}

//map = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT
//create = map | GL_DYNAMIC_STORAGE_BIT
// void* map();
//void* ptr();
//void unmap();