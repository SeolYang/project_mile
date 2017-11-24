#pragma once

#include "VertexBufferDX11.h"
#include "IndexBufferDX11.h"

namespace Mile
{
   class RendererDX11;
   class MEAPI Mesh
   {
   public:
      Mesh( RendererDX11* renderer, const std::string& name, const String& modelPath) :
         m_name( name ),
         m_modelPath( modelPath ),
         m_renderer( renderer )
      {
      }

      ~Mesh( )
      {
         SafeDelete( m_vertexBuffer );
         SafeDelete( m_indexBuffer );
      }

      template <typename Vertex>
      bool Init( const std::vector<Vertex>& verticies, const std::vector<unsigned int>& indices )
      {
         if ( m_indexBuffer != nullptr || m_indexBuffer != nullptr )
         {
            return false;
         }

         m_indexBuffer = new IndexBufferDX11( m_renderer );
         if ( !m_indexBuffer->Init( indices ) )
         {
            SafeDelete( m_indexBuffer );
            return false;
         }

         m_vertexBuffer = new VertexBufferDX11( m_renderer );
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

      std::string GetName( ) const { return m_name; }
      String GetModelPath( ) const { return m_modelPath; }

   private:
      RendererDX11*     m_renderer;
      IndexBufferDX11*  m_indexBuffer;
      VertexBufferDX11* m_vertexBuffer;

      std::string       m_name;
      String            m_modelPath;

   };
}