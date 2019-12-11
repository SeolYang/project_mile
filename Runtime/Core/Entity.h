#pragma once

#include "Helper.h"
#include "Component\Component.h"
#include "Transform.h"

namespace Mile
{
   class Context;
   class World;
   class Component;
   class Entity
   {
      friend World;
      friend class ModelLoader;

   private:
      Entity( World* world, const String& name );

   public:
      ~Entity( );

	  json Serialize() const;
      // Null world entitiy must not deserialized.
      void DeSerialize( const json& jsonData );

      /**
      * @brief    Entity를 초기화 합니다. ( 이 시점에서 Entity의 Transform 이 설정됩니다. )
      * @param    Entity에 적용할 Transform 컴포넌트
      */
      bool Init( );

      /**
      * @brief    템플릿 파라미터로 주어진 형식의 컴포넌트를 추가합니다.
      * @return   추가된 컴포넌트
      */
      template <typename Ty> Ty* AddComponent( );

      /**
      * @brief    주어진 컴포넌트가 Entity에 등록되어있다면 제거합니다.
      * @param    Entity에 등록되어있는 컴포넌트
      */
      template <typename Ty> void RemoveComponent( Ty* target );

      /**
      * @brief    템플릿 파라미터로 주어진 형식의 모든 컴포넌트들을 제거합니다. ( 단 메모리의 할당해제는 일어나지 않습니다. )
      */
      template <typename Ty> void RemoveComponents( );

      /**
      * @brief    템플릿 파라미터로 주어진 형식의 컴포넌트중 가장 먼저 추가된 컴포넌트를 반환합니다.
      * @return   가장 먼저 추가된 해당 형식의 컴포넌트
      */
      template <typename Ty> Ty* GetComponent( );

      /**
      * @brief    Entitiy에 추가되어있는 모든 컴포넌트들을 반환합니다.
      * @return   Entitiy에 추가되어있는 컴포넌트 들의 배열
      */
      template <typename Ty> std::vector<Ty*> GetComponents( );

      /**
      * @brief    템플릿 파라미터로 주어진 형식의 컴포넌트가 Entity에 추가되어있는지 확인합니다.
      * @return   해당 형식의 컴포넌트가 추가 여부
      */
      template <typename Ty> bool HasComponent( ) const;

      /**
      * @brief    Entity가 활성화 되어있는지 확인합니다.
      * @return   Entity의 활성화 여부
      */
      bool IsActive( ) const { return m_bIsActive; }

      /**
      * @brief    Entity를 활성화 시키거나 비 활성화 시킵니다.
      * @param    Entity의 활성화 여부
      */
      void SetActive( bool bIsActive );

      /**
      * @brief    Entity에 등록되어있는 Transform 컴포넌트를 반환합니다.
      * @return   Entity의 Transform 컴포넌트
      */
      Transform* GetTransform( ) { return m_transform; }

      void SetTransform( const Transform& transform );

      /**
      * @brief    Entity에 설정되어있는 이름을 반환합니다.
      * @return   Entity의 이름
      */
      String GetName( ) const { return m_name; }

      /**
      * @brief    Entity에 새로운 이름을 설정해줍니다.
      * @param    Entity에 설정해줄 새로운 이름
      */
      void SetName( const String& name ) { m_name = name; }

      bool AttachChild( Entity* child );
      bool DetachChild( Entity* child );

      Entity* GetParent( ) { return m_parent; }
      Entity* GetChildByName( const String& name );
      bool HasParent( ) { return m_parent != nullptr; }

      std::vector<Entity*> GetChildren( ) const { return m_children; }

      void Start( );
      void Update( );
      void OnEnable( );
      void OnDisable( );

      World* GetWorld( ) const { return m_world; }
      Context* GetContext( ) const { return m_context; }

   protected:
      Context*                  m_context;
      World*                    m_world;
      bool                      m_bIsActive;
      Transform*                m_transform;

   private:
      std::vector<Component*>   m_components;
      std::vector<Entity*>      m_children;
      Entity*                   m_parent;
      String                    m_name;

   };

   template <typename Ty>
   Ty* Entity::AddComponent( )
   {
      Component* component = new Ty( this );
      m_components.push_back( component );

      component->Reset( );
      component->SetActive( true );

      return static_cast< Ty* >( component );
   }

   template <typename Ty>
   void Entity::RemoveComponent( Ty* target )
   {
      if ( target != nullptr )
      {
         for ( auto itr = m_components.begin( ); itr != m_components.end( ); ++itr )
         {
            SafeDelete( target );
            m_components.erase( itr );
            break;
         }
      }
   }

   template <typename Ty>
   void Entity::RemoveComponents( )
   {
      for ( auto itr = m_components.begin( ); itr != m_components.end( ); ++itr )
      {
         Component* foundComponent = *itr;
         if ( typeid( Ty ) == typeid ( *( foundComponent ) ) )
         {
            SafeDelete( foundComponent );
            m_components.erase( itr );
         }
      }
   }

   template <typename Ty>
   Ty* Entity::GetComponent( )
   {
      for ( auto component : m_components )
      {
         if ( typeid( Ty ) == typeid( *component ) )
         {
            return reinterpret_cast<Ty*>( component );
         }
      }

      return nullptr;
   }

   template <typename Ty>
   std::vector<Ty*> Entity::GetComponents( )
   {
      std::vector<Ty*> tempArr{ };

      for ( auto component : m_components )
      {
         if ( typeid( Ty ) == typeid( *component ) )
         {
            tempArr.push_back( component );
         }
      }

      return std::move( tempArr );
   }

   template <typename Ty>
   bool Entity::HasComponent( ) const
   {
      return ( ( GetComponent<Ty>( ) ) != nullptr );
   }
}
