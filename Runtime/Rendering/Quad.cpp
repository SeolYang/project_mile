#include "Quad.h"
#include "Math\Vector4.h"

namespace Mile
{
   bool Quad::Init( float minX, float minY,
                    float maxX, float maxY )
   {
      std::vector<Vector4> vertices = {
         Vector4( 0.0f, maxY, 0.0f, 1.0f ),
         Vector4( 0.0f, 0.0f, 0.0f, 1.0f ),
         Vector4( maxX, maxY, 0.0f, 1.0f ),
         Vector4( maxX, 0.0f, 0.0f, 1.0f ) };

      std::vector<unsigned int> indices = {
         0, 1, 2,
         2, 1, 3 };

      return Mesh::Init<Vector4>( vertices, indices );
   }
}