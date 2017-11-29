#pragma once

#include "Math/Vector3.h"
#include "Math/Vector2.h"
#include "Math/Quaternion.h"
#include "Math/Matrix.h"

namespace Mile
{
   enum class TransformSpace
   {
      World,
      Local
   };

   class Entity;
   class MEAPI alignas( 16 ) Transform
   {
   public:
      Transform( Entity* entity, const Vector3& position, const Vector3& scale, const Quaternion& rotation ) :
         m_entity( entity ),
         m_position( position ),
         m_scale( scale ),
         m_rotation( rotation ),
         m_bDirtyFlag( true ),
         m_parent( nullptr )
      {
         ForceUpdateMatrix( );
      }

      Transform( Entity* entity ) :
         Transform( entity, Vector3( ), Vector3( 1.0f, 1.0f, 1.0f ), Quaternion( ) )
      {
      }

      std::string Serialize( ) const
      {
         return "{ \"Position\":" + m_position.Serialize( ) +
            ", \"Scale\":" + m_scale.Serialize( ) +
            ", \"Rotation\":" + m_rotation.Serialize( ) + " }";
      }

      void DeSerialize( const json& jsonObj )
      {
         m_position.DeSerialize( jsonObj[ "Position" ] );
         m_scale.DeSerialize( jsonObj[ "Scale" ] );
         m_rotation.DeSerialize( jsonObj[ "Rotation" ] );
         ForceUpdateMatrix( );
      }

      Vector3 GetPosition( TransformSpace space = TransformSpace::Local ) const
      {
         if ( space == TransformSpace::World && HasParent( ) )
         {
            return m_position * m_worldMatrix;
         }

         return m_position;
      }

      void SetPosition( const Vector3& position, TransformSpace space = TransformSpace::Local, bool bImmediatelyUpdateMatrix = true )
      {
         if ( space == TransformSpace::World && HasParent( ) )
         {
            m_position = position * m_parent->m_worldMatrix.Inversed( );
         }
         else
         {
            m_position = position;
         }

         if ( bImmediatelyUpdateMatrix )
         {
            ForceUpdateMatrix( );
         }
         else
         {
            m_bDirtyFlag = true;
         }
      }

      Vector3 GetScale( TransformSpace space = TransformSpace::Local ) const
      {
         if ( space == TransformSpace::World && HasParent( ) )
         {
            return m_scale * m_parent->GetScale( space );
         }

         return m_scale;
      }

      void SetScale( const Vector3& scale, TransformSpace space = TransformSpace::Local, bool bImmediatelyUpdateMatrix = true )
      {
         if ( space == TransformSpace::World && HasParent( ) )
         {
            m_scale = scale / m_parent->GetScale( space );
         }
         else
         {
            m_scale = scale;
         }
         
         if ( bImmediatelyUpdateMatrix )
         {
            ForceUpdateMatrix( );
         }
         else
         {
            m_bDirtyFlag = true;
         }
      }

      Quaternion GetRotation( TransformSpace space = TransformSpace::Local ) const
      {
         if ( space == TransformSpace::World && HasParent( ) )
         {
            return m_rotation.Rotated( m_parent->GetRotation( space ) );
         }
         else
         {
            return m_rotation;
         }
      }

      void Translate( const Vector3& vec, TransformSpace space = TransformSpace::Local, bool bImmediatelyUpdateMatrix = true )
      {
         if ( space == TransformSpace::World && HasParent( ) )
         {
            m_position += ( vec * m_parent->m_worldMatrix );
         }
         else
         {
            m_position += vec;
         }
         
         if ( bImmediatelyUpdateMatrix )
         {
            ForceUpdateMatrix( );
         }
         else
         {
            m_bDirtyFlag = true;
         }
      }

      void Scale( const Vector3& scale, TransformSpace space = TransformSpace::Local, bool bImmediatelyUpdateMatrix = true )
      {
         if ( space == TransformSpace::World && HasParent( ) )
         {
            m_scale *= scale / m_parent->GetScale( space );
         }
         else
         {
            m_scale *= scale;
         }
         
         if ( bImmediatelyUpdateMatrix )
         {
            ForceUpdateMatrix( );
         }
         else
         {
            m_bDirtyFlag = true;
         }
      }

      void Scale( float factor, bool bImmediatelyUpdateMatrix = true )
      {
         m_scale *= factor;
         if ( bImmediatelyUpdateMatrix )
         {
            ForceUpdateMatrix( );
         }
         else
         {
            m_bDirtyFlag = true;
         }
      }

      void Rotate( const Quaternion& rot, TransformSpace space = TransformSpace::Local, bool bImmediatelyUpdateMatrix = true )
      {
         if ( space == TransformSpace::World && HasParent( ) )
         {
            m_rotation.Rotate( rot.Rotated( m_parent->GetRotation( space ).Inverse( ) ) );
         }
         else
         {
            m_rotation.Rotate( rot );
         }

         if ( bImmediatelyUpdateMatrix )
         {
            ForceUpdateMatrix( );
         }
         else
         {
            m_bDirtyFlag = true;
         }
      }

      Transform* GetParent( ) const;

      bool HasParent( ) const { return m_parent != nullptr; }

      Entity* GetEntity( ) const { return m_entity; }

      Matrix GetWorldMatrix( )
      {
         UpdateMatrix( );

         return m_worldMatrix;
      }

      /*
      * No guarantee that matrix is updated.
      * If you called transform functions with immediately update flag, then is able to receive updated matrix.
      */
      Matrix GetWorldMatrix( ) const
      {
         return m_worldMatrix;
      }

      Matrix GetLocalMatrix( )
      {
         UpdateMatrix( );

         return m_localMatrix;
      }

      /*
      * No guarantee that matrix is updated.
      * If you called transform functions with immediately update flag, then is able to receive updated matrix.
      */
      Matrix GetLocalMatrix( ) const
      {
         return m_localMatrix;
      }

      void UpdateMatrix( )
      {
         if ( m_bDirtyFlag )
         {
            ForceUpdateMatrix( );
         }
      }

   private:
      void SetParent( Transform* parent )
      {
         m_parent = parent;
      }

      void ForceUpdateMatrix( );

   private:
      /* Local Transform **/
      Vector3                 m_position;
      Vector3                 m_scale;
      Quaternion              m_rotation;

      Matrix                  m_localMatrix;
      Matrix                  m_worldMatrix;

      Entity*                 m_entity;
      Transform*              m_parent;

      bool                    m_bDirtyFlag;
      friend Entity;
   };

   using TransformPtr = std::unique_ptr<Transform>;
   using WeakTransformPtr = std::weak_ptr<Transform>;
}