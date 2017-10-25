#pragma once
#include "Resource/Resource.h"

namespace Mile
{
   class ResourceDX11 : public Resource
   {
   public:
      ResourceDX11( Context* context, const std::string& path, ResourceType resourceType );

      virtual class ID3D11Resource* GetResource( ) = 0;

   };
}