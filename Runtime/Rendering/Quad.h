#pragma once

#include "Mesh.h"
#include "Math\Vector2.h"

namespace Mile
{
   class MEAPI Quad : public Mesh
   {
   public:
      Quad( RendererDX11* renderer ) :
         Mesh( renderer, "Quad", TEXT( "" ) )
      {
      }

      bool Init( float minX, float minY,
                 float maxX, float maxY );
   };
}