#include "pch.h"
#include "ImmutableTexture.h"

using namespace std;

namespace Axodox::Graphics::D3D12
{
  ImmutableTexture::ImmutableTexture(const ResourceAllocationContext& context, const std::filesystem::path& path)
  {
    auto textureData = TextureData::FromFile(path);
    _texture = context.ResourceAllocator->CreateTexture(textureData.Definition());

    _allocatedSubscription = _texture->Allocated([this, context, data = move(textureData)](Resource* resource) {
      context.ResourceUploader->EnqueueUploadTask(resource, &data);
      _view = context.DescriptorHeap->CreateShaderResourceView(resource);
      });
  }

  ImmutableTexture::operator D3D12_GPU_VIRTUAL_ADDRESS() const
  {
    return *_view;
  }
}