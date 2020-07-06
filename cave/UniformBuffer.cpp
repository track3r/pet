#include "pch.h"

#include "UniformBuffer.h"

UniformBuffer::~UniformBuffer()
{
    glDeleteBuffers(1, &_ubo);
}

bool UniformBuffer::init(int size)
{
    glGenBuffers(1, &_ubo);
    CheckGlError();
    glBindBuffer(GL_UNIFORM_BUFFER, _ubo);
    CheckGlError();
    glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
    CheckGlError();
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    CheckGlError();
    return true;
}

void UniformBuffer::update(int offset, int size, const void* data)
{
    glBindBuffer(GL_UNIFORM_BUFFER, _ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
    CheckGlError();
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::bind(int bindingPoint)
{
    assert(_ubo != GL_INVALID_INDEX);
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, _ubo);
    CheckGlError();
}