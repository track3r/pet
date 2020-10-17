#include "pch.h"
#include "RenderGeometryManager.h"
#include "Renderer.h"

void ExternalAllocator::init(uint32_t maxSize)
{
    _maxSize = maxSize;
    _ptr = 0;
    _lastSize = 0;
}

uint32_t ExternalAllocator::allocate(uint32_t size)
{
    assert(size <= (_maxSize - _ptr));
    if (size > _maxSize - _ptr)
    {
        return -1;
    }

    _lastSize = size;
    uint32_t ret = _ptr;
    _ptr += size;

    return ret;
}

void ExternalAllocator::free(uint32_t offset)
{
    if (_ptr - _lastSize == offset)
    {
        _ptr = offset;
    }
}

void ExternalAllocator::reset()
{
    _ptr = 0;
    _lastSize = 0;
}


bool RenderGeometryManager::init(uint32_t indexSize, uint32_t vertexSize)
{
    _indexAllocator.init(sizeof(uint32_t) * indexSize);
    //_index._mappedUpdate = true;
    _index.init(GpuBuffer::Index, indexSize * sizeof(uint32_t), "WorldGeoIndex");

    _vertexAllocator.init(_vertexFormat.size() * vertexSize);
    //_vertex._mappedUpdate = true;
    _vertex.init(GpuBuffer::Vertex, _vertexFormat.size() * vertexSize, "WorldGeoIndexVertex");

    glGenVertexArrays(1, &_vao);
    RenderElement::setupVao(_vao, _vertex, _vertexFormat);
    RenderContext::oglContext->bindBuffer(_index);
    return true;
}

bool RenderGeometryManager::allocate(meshAlloc_t& output, uint32_t indexNum, uint32_t vertexNum)
{
    uint32_t indexSize = sizeof(uint32_t) * indexNum;
    uint32_t vertexSize = _vertexFormat.size() * vertexNum;

    uint32_t indexOffset = _indexAllocator.allocate(indexSize);
    if (indexOffset == -1)
    {
        return false;
    }

    uint32_t vertexOffset = _vertexAllocator.allocate(vertexSize);
    if (vertexOffset == -1)
    {
        _indexAllocator.free(indexOffset);
    }

    output.indexOffset = indexOffset;
    output.vertexOffset = vertexOffset;

    return true;
}

bool RenderGeometryManager::allocate(RenderElement& output, uint32_t indexNum, uint32_t vertexNum)
{
    meshAlloc_t alloc;
    if (!allocate(alloc, indexNum + 1, vertexNum + 1))
    {
        return false;
    }

    output = RenderElement(_index, alloc.indexOffset / sizeof(uint32_t), indexNum, _vertex, alloc.vertexOffset / _vertexFormat.size(), vertexNum, _vertexFormat);
    output._vao = _vao;
    return true;
}

void RenderGeometryManager::free(uint32_t allocId)
{

}

void RenderGeometryManager::reset()
{
    _vertexAllocator.reset();
    _indexAllocator.reset();
}


void RenderGeometryManager::updateIndex(uint32_t offset, uint32_t size, void* data)
{
    _index.update(offset, size, data);
}

void RenderGeometryManager::updateVertex(uint32_t offset, uint32_t size, void* data)
{
    _vertex.update(offset, size, data);
}