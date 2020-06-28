#pragma once

#include "GlHeaders.h"
#include "GlmIncludes.h"

class UniformBuffer
{
public:
    ~UniformBuffer();
    bool init(int size);
    void update(int offset, int size, void* data);
    void bind(int bindingPoint);

    GLuint _ubo;
    uint16_t _size;
};