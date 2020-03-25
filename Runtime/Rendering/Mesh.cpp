#include "Rendering/Mesh.h"

namespace Mile
{
   bool Mesh::Bind(ID3D11DeviceContext& deviceContext, unsigned int startSlot)
   {
      bool bIsInitialized = m_indexBuffer != nullptr && m_vertexBuffer != nullptr;
      if (!bIsInitialized)
      {
         return false;
      }

      bool bIsBinded = m_indexBuffer->Bind(deviceContext) &&
         m_vertexBuffer->Bind(deviceContext, startSlot);
      if (!bIsBinded)
      {
         return false;
      }

      return true;
   }
}