#pragma once
#include "Rendering/Mesh.h"
#include "Math/Vector2.h"

namespace Mile
{
   class MEAPI Quad : public Mesh
   {
   public:
      Quad(RendererDX11* renderer) :
         Mesh(renderer, TEXT("Quad"), TEXT(""), EStaticMeshType::Quad)
      {
      }

      bool Init(float minX = -1.f, float minY = -1.f,
         float maxX = 1.f, float maxY = 1.f);
   };
}