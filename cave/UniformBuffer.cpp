#include "pch.h"

#include "UniformBuffer.h"

UniformBuffer::~UniformBuffer()
{
    glDeleteBuffers(1, &_ubo);
}

bool UniformBuffer::init(int size)
{
    _size = size;
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

void UniformBuffer::bindForUpdate()
{
    glBindBuffer(GL_UNIFORM_BUFFER, _ubo);
}

void UniformBuffer::unbindForUpdate()
{
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::update(int offset, int size, const void* data)
{
    //glBindBuffer(GL_UNIFORM_BUFFER, _ubo);
    
    //glNamedBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
    //glNamedBufferData()
    
    
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
    
    
    //void* ptr = glMapBufferRange(GL_UNIFORM_BUFFER, offset, size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_WRITE_BIT);
    //memcpy(ptr, data, size);
    //glUnmapBuffer(GL_UNIFORM_BUFFER);
    
    //CheckGlError();
    //glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::bind(int bindingPoint)
{
    assert(_ubo != GL_INVALID_INDEX);
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, _ubo);
    CheckGlError();
}