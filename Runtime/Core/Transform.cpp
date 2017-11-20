#include "Transform.h"
#include "Entity.h"

namespace Mile
{
   Transform* Transform::GetParent( ) const
   {
      Entity* parent = m_entity->GetParent( );
      if ( parent == nullptr )
      {
         return nullptr;
      }
      
      return parent->GetTransform( );
   }

   void Transform::ForceUpdateMatrix( )
   {
      m_localMatrix = Matrix::CreateTransformMatrix( m_scale, m_rotation, m_position );

      if ( m_parent != nullptr )
      {
         m_worldMatrix = m_localMatrix * m_parent->m_worldMatrix;
      }
      else
      {
         m_worldMatrix = m_localMatrix;
      }

      std::vector<Entity*> children = std::move( m_entity->GetChildren( ) );
      for ( auto child : children )
      {
         child->GetTransform( )->ForceUpdateMatrix( );
      }

      m_bDirtyFlag = false;
   }
}