#include "pch.h"

#include "UniformBuffer.h"

UniformBuffer::~UniformBuffer()
{
    glDeleteBuffers(1, &_ubo);
}

bool UniformBuffer::init(int size)
{
    glGenBuffers(1, &_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, _ubo);
    glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_DYNAMIC_DRAW); // allocate 152 bytes of memory
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    return false;
}

void UniformBuffer::update(int offset, int size, void* data)
{

}

void UniformBuffer::bind(int bindingPoint)
{

}