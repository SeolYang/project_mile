#pragma once
#include "Rendering/Mesh.h"

namespace Mile
{
   class MEAPI Cube : public Mesh
   {
   public:
      Cube(RendererDX11* renderer) :
         Mesh(renderer, TEXT("Cube"), TEXT(""))
      {
      }

      bool Init(const Vector3& min, const Vector3& max);

   };
}