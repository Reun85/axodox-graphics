#include "pch.h"
#include "Mesh.h"

using namespace Axodox::Infrastructure;
using namespace std;

namespace Axodox::Graphics::D3D12
{
  Mesh::Mesh(ResourceAllocator& allocator, ResourceUploader& uploader, MeshDescription&& description) :
    _vertexBuffer(allocator.CreateBuffer(BufferDefinition(description.Vertices))),
    _indexBuffer(allocator.CreateBuffer(BufferDefinition(description.Indices))),
    _topology(D3D12_PRIMITIVE_TOPOLOGY(description.Topology)),
    _vertexCount(description.Vertices.ItemCount()),
    _indexCount(description.Indices.ItemCount())
  {
    if (_vertexBuffer)
    {
      _verticesAllocatedSubscription = _vertexBuffer->Allocated([this, buffer = move(description.Vertices), &uploader](Resource* resource) {
        uploader.EnqueueUploadTask(resource, &buffer);
        _vertexBufferView = { (*resource)->GetGPUVirtualAddress(), buffer.ByteCount(), buffer.ItemSize() };
        });
    }

    if (_indexBuffer)
    {
      _indicesAllocatedSubscription = _indexBuffer->Allocated([this, buffer = move(description.Indices), &uploader](Resource* resource) {
        uploader.EnqueueUploadTask(resource, &buffer);
        _indexBufferView = { (*resource)->GetGPUVirtualAddress(), buffer.ByteCount(), GetIndexFormat(buffer.ItemSize()) };
        });
    }
  }

  void Mesh::Draw(CommandAllocator& allocator, uint32_t instanceCount)
  {
    if (bool(_vertexBuffer) != bool(_vertexBufferView) || bool(_indexBuffer) != bool(_indexBufferView) || !instanceCount) return;

    if (_vertexBuffer) allocator->IASetVertexBuffers(0, 1, &*_vertexBufferView);
    if (_indexBuffer) allocator->IASetIndexBuffer(&*_indexBufferView);
    allocator->IASetPrimitiveTopology(_topology);
    
    if (_indexBuffer)
    {
      allocator->DrawIndexedInstanced(_indexCount, instanceCount, 0u, 0u, 0u);
    }
    else
    {
      allocator->DrawInstanced(_vertexCount, instanceCount, 0u, 0u);
    }
  }

  DXGI_FORMAT Mesh::GetIndexFormat(uint32_t size)
  {
    switch (size)
    {
    case 4:
      return DXGI_FORMAT_R32_UINT;
    case 2:
      return DXGI_FORMAT_R16_UINT;
    default:
      return DXGI_FORMAT_UNKNOWN;
    }
  }
}