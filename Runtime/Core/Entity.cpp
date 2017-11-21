#include "Entity.h"
#include "Context.h"
#include "Component/Component.h"
#include "ComponentRegister.h"

namespace Mile
{
   Entity::Entity( Context* context, const String& name ) :
      m_context( context ),
      m_name( name ),
      m_bIsActive( false ),
      m_transform( new Transform( this ) ),
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

   std::string Entity::Serialize( ) const
   {
      std::string res = "{ \"Name\": \"" + Mile::WString2String(m_name) +"\", \"IsActivated\": " + BoolSerialize( m_bIsActive ) +
         ", \"Transform\": " + m_transform->Serialize( ) + ", \"Components\": [  ";
      for ( auto comp : m_components )
      {
         res += comp->Serialize( ) + ", ";
      }
      res[ res.length( ) - 2 ] = ']';

      res += ", \"Children\": [  ";
      for ( auto child : m_children )
      {
         res += child->Serialize( ) + ", ";
      }
      res[ res.length( ) - 2 ] = ']';

      res += '}';

      return res;
   }

   void Entity::DeSerialize( const json& jsonData )
   {
      m_name = Mile::String2WString( jsonData[ "Name" ] );
      m_bIsActive = jsonData[ "IsActivated" ];

      m_transform->DeSerialize( jsonData[ "Transform" ] );

      std::vector<json> components = jsonData[ "Components" ];
      for ( auto component : components )
      {
         std::string type = component[ "Type" ];
         Component* compInst = ComponentRegister::GetInstance( ).Acquire( type, m_context );
         compInst->DeSerialize( component );
         m_components.push_back( compInst );
      }

      std::vector<json> children = jsonData[ "Children" ];
      for ( auto child : children )
      {
         Entity* tempChild = new Entity( m_context, TEXT("") );
         tempChild->DeSerialize( child );
         this->AttachChild( tempChild );
      }
   }

   bool Entity::Init( )
   {
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