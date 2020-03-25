#pragma once
#include "Rendering/Mesh.h"
#include "Math/Vector2.h"

namespace Mile
{
   class MEAPI Quad : public Mesh
   {
   public:
      Quad(RendererDX11* renderer) :
         Mesh(renderer, TEXT("Quad"), TEXT(""))
      {
      }

      bool Init(float minX, float minY,
         float maxX, float maxY);
   };
}