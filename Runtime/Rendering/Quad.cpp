#include "Rendering/Quad.h"
#include "Math/Vector4.h"
#include "Math/Vertex.h"

namespace Mile
{
   bool Quad::Init(float minX, float minY,
      float maxX, float maxY)
   {
      std::vector<VertexPosTex> vertices = {
      { Vector4(minX, maxY, 0.0f, 1.0f), Vector2(0.0f, 1.0f) },
      { Vector4(maxX, maxY, 0.0f, 1.0f), Vector2(1.0f, 1.0f) },
      { Vector4(minX, minY, 0.0f, 1.0f), Vector2(0.0f, 0.0f) },
      { Vector4(maxX, minY, 0.0f, 1.0f), Vector2(1.0f, 0.0f) } };

      std::vector<unsigned int> indices = {
         0, 1, 2,
         2, 1, 3
      };

      return Mesh::Init<VertexPosTex>(vertices, indices);
   }
}