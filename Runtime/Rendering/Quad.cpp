#include "Quad.h"
#include "Math\Vector4.h"

namespace Mile
{
   bool Quad::Init( const Vector2& topLeft, const Vector2& topRight,
                    const Vector2& bottomLeft, const Vector2& bottomRight )
   {
      std::vector<Vector4> vertices = {
         Vector4( bottomLeft.x, bottomLeft.y, 0.0f, 1.0f ),
         Vector4( topLeft.x, topLeft.y, 0.0f, 1.0f ),
         Vector4( bottomRight.x, bottomRight.y, 0.0f, 1.0f ),
         Vector4( topRight.x, topRight.y, 0.0f, 1.0f ) };

      std::vector<unsigned int> indices = {
         0, 1, 2,
         2, 1, 3 };

      return Mesh::Init<Vector4>( vertices, indices );
   }
}