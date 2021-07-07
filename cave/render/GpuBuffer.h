#pragma once

#include "GlHeaders.h"
#include "GlmIncludes.h"

class GpuBuffer
{
public:
	enum Type
	{
		None, Vertex, Index, Uniform, Indirect, MaxType
	};
	GpuBuffer() {}
	~GpuBuffer();

	bool init(Type type, uint32_t size, const char* debugName = nullptr);
	bool init(const GpuBuffer& ref, uint32_t offset, uint32_t size);

	void update(uint32_t offset, uint32_t size, const void* data) const;
	GLuint getGlObject() const { return _apiObject; }
	Type getType() const { return _type; }
	bool _mappedUpdate = false;
private:
	Type _type = None;
	uint8_t _flags = 0;
	GLuint _apiObject = 0;
	uint32_t _size = 0;
	uint32_t _offset = 0;
	bool _isReference = false;
	const char* _debugName = nullptr;

	//GpuBuffer(const GpuBuffer& o);
};

template<class T>
class TypedUniformBuffer : public GpuBuffer
{
public:
    bool init(const char* debugName)
    {
        return GpuBuffer::init(GpuBuffer::Uniform, (uint32_t) sizeof(T), debugName);
    }
    void updateFull(const T& data)
    {
        update(0, sizeof(T), &data);
    }
};