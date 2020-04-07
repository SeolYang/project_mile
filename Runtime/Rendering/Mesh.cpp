#include "Rendering/Mesh.h"

namespace Mile
{
   bool Mesh::Bind(ID3D11DeviceContext& deviceContext, unsigned int startSlot)
   {
      if (RenderObject::IsBindable())
      {
         bool bIsBinded = m_indexBuffer->Bind(deviceContext) &&
            m_vertexBuffer->Bind(deviceContext, startSlot);
         if (bIsBinded)
         {
            return true;
         }
      }

      return false;
   }
}