#pragma once

#include "GlHeaders.h"
#include "GlmIncludes.h"

class UniformBuffer
{
public:
    ~UniformBuffer();
    bool init(int size);
    void update(int offset, int size, const void* data);
    void bind(int bindingPoint);

    GLuint _ubo = GL_INVALID_INDEX;
    uint16_t _size;
};

template<class T>
class TypedUniformBuffer : public UniformBuffer
{
public:
    bool init()
    {
        return UniformBuffer::init((int)sizeof(T));
    }
    void updateFull(const T& data)
    {
        update(0, sizeof(T), &data);
    }
};