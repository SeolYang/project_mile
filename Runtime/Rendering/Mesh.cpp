#include "Mesh.h"

namespace Mile
{
   bool Mesh::Bind( unsigned int startSlot )
   {
      bool bIsInitialized = m_indexBuffer != nullptr && m_vertexBuffer != nullptr;
      if ( !bIsInitialized )
      {
         return false;
      }

      bool bIsBinded = m_indexBuffer->Bind( ) && m_vertexBuffer->Bind( startSlot );
      if ( !bIsBinded )
      {
         return false;
      }

      return true;
   }
}