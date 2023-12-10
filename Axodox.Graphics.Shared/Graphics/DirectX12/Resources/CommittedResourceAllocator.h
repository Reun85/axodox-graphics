#pragma once
#include "ResourceAllocator.h"

namespace Axodox::Graphics::D3D12
{
  class CommittedResourceAllocator : public ResourceAllocator
  {
  public:
    using ResourceAllocator::ResourceAllocator;

    virtual void AllocateResources(ResourceSpan resources) override;    
  };
}