#include "GameFramework/Transform.h"
#include "GameFramework/Entity.h"

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

   Vector3 Transform::GetForward( TransformSpace space ) const
   {
      //Vector3 res = GetRotation( space ) * Vector3::Forward( );
      //res.Normalize( );
      //return res;

      Vector4 res = Vector4::Forward( ) * Matrix::CreateRotation( GetRotation( space ) );
      Vector3 ret{ res.x, res.y, res.z };
      ret.Normalize( );
      return ret;
   }

   Vector3 Transform::GetUp( TransformSpace space ) const
   {
      Vector3 res = GetRotation( space ) * Vector3::Up( );
      res.Normalize( );
      return res;
   }
}