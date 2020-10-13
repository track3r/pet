#pragma once

#include "GpuBuffer.h"
#include "VertexFormat.h"
#include "RenderElement.h"

class ExternalAllocator
{
public:
    void init(uint32_t maxSize);
    uint32_t allocate(uint32_t size);
    void free(uint32_t);
    void reset();

private:
    uint32_t _maxSize = 0;
    uint32_t _ptr = 0;
    uint32_t _lastSize = 0;
};

class RenderGeometryManager
{
public:
    struct meshAlloc_t
    {
        uint32_t indexOffset;
        uint32_t vertexOffset;
    };
    RenderGeometryManager(const VertexFormat& format)
        : _vertexFormat(format)
    {

    }
    bool init(uint32_t indexSize, uint32_t vertexSize);
    bool allocate(meshAlloc_t& output, uint32_t indexNum, uint32_t vertexNum);
    bool allocate(RenderElement& output, uint32_t indexNum, uint32_t vertexNum);
    void free(uint32_t allocId);
    void reset();

    void updateIndex(uint32_t offset, uint32_t size, void* data);
    void updateVertex(uint32_t offset, uint32_t size, void* data);

private:
    RenderGeometryManager(const RenderGeometryManager& other) 
        :_vertexFormat(c_debugVf)
        {}

    GpuBuffer _vertex;
    GpuBuffer _index;
    ExternalAllocator _vertexAllocator;
    ExternalAllocator _indexAllocator;
    const VertexFormat& _vertexFormat;
    GLuint _vao = 0;
};