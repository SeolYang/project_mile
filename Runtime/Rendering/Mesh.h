#pragma once

#include "VertexBufferDX11.h"
#include "IndexBufferDX11.h"

namespace Mile
{
   class RendererDX11;
   class MEAPI Mesh
   {
   public:
      // @TODO Vertex/Index Buffer Initialize
      Mesh( RendererDX11* renderer) :
         m_renderer( renderer )
      {
      }

      ~Mesh( )
      {
         SafeDelete( m_vertexBuffer );
         SafeDelete( m_indexBuffer );
      }

      template <typename Vertex>
      bool Init( const std::vector<Vertex>& verticies, const std::vector<unsigned int>& indicies )
      {
         if ( m_indexBuffer != nullptr || m_indexBuffer != nullptr )
         {
            return false;
         }

         m_indexBuffer = new IndexBufferDX11( m_renderer );
         if ( !m_indexBuffer->Init( indicies ) )
         {
            SafeDelete( m_indexBuffer );
            return false;
         }

         m_vertexBuffer = new VertexBufferDX11( m_vertexBuffer );
         if ( !m_vertexBuffer->Init<Vertex>( verticies ) )
         {
            SafeDelete( m_vertexBuffer );
            return false;
         }

         return true;
      }

      bool Bind( unsigned int startSlot );

      IndexBufferDX11* GetIndexBuffer( ) { return m_indexBuffer; }
      VertexBufferDX11* GetVertexBuffer( ) { return m_vertexBuffer; }


   private:
      RendererDX11*  m_renderer;
      IndexBufferDX11* m_indexBuffer;
      VertexBufferDX11* m_vertexBuffer;

   };
}