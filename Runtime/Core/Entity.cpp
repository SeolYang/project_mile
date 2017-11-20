#include "Entity.h"
#include "Context.h"
#include "Component/Component.h"

namespace Mile
{
   Entity::Entity( Context* context ) :
      m_context( context ),
      m_bIsActive( false ),
      m_transform( nullptr ),
      m_parent( nullptr )
   {
   }

   Entity::~Entity( )
   {
      SafeDelete( m_transform );
      for ( auto component : m_components )
      {
         SafeDelete( component );
      }
      for ( auto child : m_children )
      {
         SafeDelete( child );
      }
   }

   bool Entity::Init( )
   {
      m_transform = new Transform( this );
      return true;
   }

   void Entity::SetActive( bool bIsActive )
   {
      if ( m_bIsActive != bIsActive )
      {
         m_bIsActive = bIsActive;
         if ( m_bIsActive )
         {
            OnEnable( );
         }
         else
         {
            OnDisable( );
         }
      }
   }

   bool Entity::AttachChild( Entity* child )
   {
      bool bIsValidPtr = child != nullptr && child != this;
      bool bIsValidChild = child->m_parent != this;
      if ( !( bIsValidPtr && bIsValidChild ) )
      {
         return false;
      }

      Entity* oldParent = child->m_parent;
      if ( oldParent != nullptr )
      {
         if ( !oldParent->DetachChild( child ) )
         {
            return false;
         }
      }

      child->m_parent = this;
      child->GetTransform( )->SetParent( m_transform );
      m_children.push_back( child );
      return true;
   }

   bool Entity::DetachChild( Entity* child )
   {
      bool bIsValidPtr = child != nullptr && child != this;
      bool bIsValidChild = child->m_parent == this;
      if ( !( bIsValidPtr && bIsValidChild ) )
      {
         return false;
      }

      m_children.erase( std::find( m_children.begin( ), m_children.end( ), child ) );
      child->GetTransform( )->SetParent( nullptr );
      return true;
   }

   void Entity::SetTransform( const Transform& transform )
   {
      ( *m_transform ) = transform;
   }

   Entity* Entity::GetChildByName( const String& name )
   {
      for ( auto child : m_children )
      {
         if ( child->GetName( ) == name )
         {
            return child;
         }
      }

      return nullptr;
   }

   void Entity::Start( )
   {
      for ( auto component : m_components )
      {
         component->Start( );
      }
   }

   void Entity::Update( )
   {
      if ( m_bIsActive )
      {
         for ( auto component : m_components )
         {
            if ( component->IsActive( ) )
            {
               component->Update( );
            }
         }

         for ( auto child : m_children )
         {
            child->Update( );
         }
      }
   }

   void Entity::OnEnable( )
   {
      for ( auto component : m_components )
      {
         if ( component->IsActive( ) )
         {
            component->OnEnable( );
         }
      }
   }

   void Entity::OnDisable( )
   {
      for ( auto component : m_components )
      {
         if ( component->IsActive( ) )
         {
            component->OnDisable( );
         }
      }
   }
}