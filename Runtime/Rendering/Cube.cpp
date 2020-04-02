#include "Rendering/Cube.h"
#include "Math/Vertex.h"

namespace Mile
{
   bool Cube::Init(const Vector3& min, const Vector3& max)
   {
      /* Vertex Structure : (Position, TexCoord, Normal) **/
      std::vector<VertexPTN> vertices = {
         { Vector4(max.x, max.y, min.z, 1.0f), Vector2(0.0f, 1.0f), Vector3(0.333333f, 0.666667f, -0.666667f) },
         { Vector4(min.x, max.y, min.z, 1.0f), Vector2(1.0f, 1.0f), Vector3(-0.816497f, 0.408248f, -0.408248f) },
         { Vector4(min.x, max.y, max.z, 1.0f), Vector2(0.0f, 1.0f), Vector3(-0.333333f, 0.666667f, 0.666667f) },
         { Vector4(max.x, max.y, max.z, 1.0f), Vector2(1.0f, 1.0f), Vector3(0.816497f, 0.408248f, 0.408248f) },
         { Vector4(max.x, min.y, min.z, 1.0f), Vector2(0.0f, 0.0f), Vector3(0.666667f, -0.666667f, -0.333333f) },
         { Vector4(min.x, min.y, min.z, 1.0f), Vector2(1.0f, 0.0f), Vector3(-0.408248f, -0.408248f, -0.816497f) },
         { Vector4(min.x, min.y, max.z, 1.0f), Vector2(0.0f, 0.0f), Vector3(-0.666667f, -0.666667f, 0.333333f) },
         { Vector4(max.x, min.y, max.z, 1.0f), Vector2(1.0f, 0.0f), Vector3(0.408248f, -0.408248f, 0.816497f) }
      };

      std::vector<UINT32> indices = {
         0, 1, 2, // 0
         0, 2, 3, // 1
         0, 4, 5, // 2
         0, 5, 1, // 3
         1, 5, 6, // 4
         1, 6, 2, // 5
         2, 6, 7, // 6
         2, 7, 3, // 7
         3, 7, 4, // 8
         3, 4, 0, // 9
         4, 7, 6, // 10
         4, 6, 5  // 11
      };

      return Mesh::Init<VertexPTN>(vertices, indices);
   }
}