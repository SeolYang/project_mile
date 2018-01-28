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
      m_localMatrix = Matrix::CreateTransformMatrix( m_position, m_scale, m_rotation );

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

   Vector3 Transform::GetForward( TransformSpace space )
   {
      UpdateMatrix( );

      Vector4 res;
      switch ( space )
      {
      case TransformSpace::Local:
         res = Vector4::Forward( ) * m_localMatrix;
         break;

      case TransformSpace::World:
         res = Vector4::Forward( ) * m_worldMatrix;
         break;
      }
      
      auto ret = Vector3( res.x, res.y, res.z );
      ret.Normalize( );

      return ret;
   }
}