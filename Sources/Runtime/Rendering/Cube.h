#pragma once
#include "Rendering/Mesh.h"

namespace Mile
{
   class MEAPI Cube : public Mesh
   {
   public:
      Cube(RendererDX11* renderer) :
         Mesh(renderer, TEXT("Cube"), TEXT(""), EStaticMeshType::Cube)
      {
      }

      bool Init(const Vector3& min = Vector3(-1.f, -1.f, -1.f), const Vector3& max = Vector3(1.f, 1.f, 1.f));

   };
}