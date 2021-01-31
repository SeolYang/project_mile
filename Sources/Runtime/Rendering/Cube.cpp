#include "Rendering/Cube.h"
#include "Math/Vertex.h"

namespace Mile
{
   bool Cube::Init(const Vector3& min, const Vector3& max)
   {
      /* Vertex Structure : (Position, TexCoord, Normal) **/
      std::vector<VertexPTN> vertices = {
         { Vector4(min.x, min.y, min.z, 1.0f), Vector2(0.0f, 1.0f), Vector3(0.0f, 0.0f, -1.0f) }, // front face
         { Vector4(min.x, max.y, min.z, 1.0f), Vector2(0.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f) },
         { Vector4(max.x, max.y, min.z, 1.0f), Vector2(1.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f) },
         { Vector4(max.x, min.y, min.z, 1.0f), Vector2(1.0f, 1.0f), Vector3(0.0f, 0.0f, -1.0f) },

         { Vector4(max.x, min.y, max.z, 1.0f), Vector2(0.0f, 1.0f), Vector3(0.0f, 0.0f, 1.0f) }, // back face
         { Vector4(max.x, max.y, max.z, 1.0f), Vector2(0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f) },
         { Vector4(min.x, max.y, max.z, 1.0f), Vector2(1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f) },
         { Vector4(min.x, min.y, max.z, 1.0f), Vector2(1.0f, 1.0f), Vector3(0.0f, 0.0f, 1.0f) },

         { Vector4(min.x, min.y, max.z, 1.0f), Vector2(0.0f, 1.0f), Vector3(-1.0f, 0.0f, 0.0f) }, // left-side face
         { Vector4(min.x, max.y, max.z, 1.0f), Vector2(0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f) },
         { Vector4(min.x, max.y, min.z, 1.0f), Vector2(1.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f) },
         { Vector4(min.x, min.x, min.z, 1.0f), Vector2(1.0f, 1.0f), Vector3(-1.0f, 0.0f, 0.0f) },

         { Vector4(max.x, min.y, min.z, 1.0f), Vector2(0.0f, 1.0f), Vector3(1.0f, 0.0f, 0.0f) }, // right-side face
         { Vector4(max.x, max.y, min.z, 1.0f), Vector2(0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f) },
         { Vector4(max.x, max.y, max.z, 1.0f), Vector2(1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f) },
         { Vector4(max.x, min.y, max.z, 1.0f), Vector2(1.0f, 1.0f), Vector3(1.0f, 0.0f, 0.0f) },

         { Vector4(min.x, max.y, min.z, 1.0f), Vector2(0.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f) }, // upper-side face
         { Vector4(min.x, max.y, max.z, 1.0f), Vector2(0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f) },
         { Vector4(max.x, max.y, max.z, 1.0f), Vector2(1.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f) },
         { Vector4(max.x, max.y, min.z, 1.0f), Vector2(1.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f) },

         { Vector4(min.x, min.y, min.z, 1.0f), Vector2(0.0f, 1.0f), Vector3(0.0f, -1.0f, 0.0f) }, // lower-side face
         { Vector4(min.x, min.y, max.z, 1.0f), Vector2(0.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f) },
         { Vector4(max.x, min.y, max.z, 1.0f), Vector2(1.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f) },
         { Vector4(max.x, min.y, min.z, 1.0f), Vector2(1.0f, 1.0f), Vector3(0.0f, -1.0f, 0.0f) }
      };

      std::vector<UINT32> indices = {
         0, 1, 2,
         0, 2, 3,

         4, 5, 6,
         4, 6, 7,

         8, 9, 10,
         8, 10, 11,

         12, 13, 14,
         12, 14, 15,

         16, 17, 18,
         16, 18, 19,

         22, 21, 20,
         23, 22, 20,
      };

      return Mesh::Init<VertexPTN>(vertices, indices);
   }
}