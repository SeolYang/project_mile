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

      bool Init( const Vector2& topLeft, const Vector2& topRight,
                 const Vector2& bottomLeft, const Vector2& bottomRight );
   };
}