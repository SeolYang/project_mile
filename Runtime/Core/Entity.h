#pragma once

#include "Helper.h"
#include "Component\Component.h"
#include "Rumia\Array.h"

namespace Mile
{
    class Context;
    class Component;
    class Transform;
    class Entity
    {
    public:
        Entity( Context* context );
        ~Entity( );

        /**
        * @brief    Entity�� �ʱ�ȭ �մϴ�. ( �� �������� Entity�� Transform �� �����˴ϴ�. )
        * @param    Entity�� ������ Transform ������Ʈ
        */
        void Initialize( Transform* transform );

        /**
        * @brief    ���ø� �Ķ���ͷ� �־��� ������ ������Ʈ�� �߰��մϴ�.
        * @return   �߰��� ������Ʈ
        */
        template <typename Ty> Ty* AddComponent( );

        /**
        * @brief    �־��� ������Ʈ�� Entity�� ��ϵǾ��ִٸ� �����մϴ�.
        * @param    Entity�� ��ϵǾ��ִ� ������Ʈ
        */
        template <typename Ty> void RemoveComponent( Ty* target );

        /**
        * @brief    ���ø� �Ķ���ͷ� �־��� ������ ��� ������Ʈ���� �����մϴ�. ( �� �޸��� �Ҵ������� �Ͼ�� �ʽ��ϴ�. )
        */
        template <typename Ty> void RemoveComponents( );

        /**
        * @brief    ���ø� �Ķ���ͷ� �־��� ������ ������Ʈ�� ���� ���� �߰��� ������Ʈ�� ��ȯ�մϴ�.
        * @return   ���� ���� �߰��� �ش� ������ ������Ʈ
        */
        template <typename Ty> Ty* GetComponent( );

        /**
        * @brief    Entitiy�� �߰��Ǿ��ִ� ��� ������Ʈ���� ��ȯ�մϴ�.
        * @return   Entitiy�� �߰��Ǿ��ִ� ������Ʈ ���� �迭
        */
        template <typename Ty> Array<Ty*> GetComponents( );

        /**
        * @brief    ���ø� �Ķ���ͷ� �־��� ������ ������Ʈ�� Entity�� �߰��Ǿ��ִ��� Ȯ���մϴ�.
        * @return   �ش� ������ ������Ʈ�� �߰� ����
        */
        template <typename Ty> bool HasComponent( ) const;

        /**
        * @brief    Entity�� Ȱ��ȭ �Ǿ��ִ��� Ȯ���մϴ�.
        * @return   Entity�� Ȱ��ȭ ����
        */
        bool IsActive( ) const { return m_bIsActive; }

        /**
        * @brief    Entity�� Ȱ��ȭ ��Ű�ų� �� Ȱ��ȭ ��ŵ�ϴ�.
        * @param    Entity�� Ȱ��ȭ ����
        */
        void SetActive( bool bIsActive );

        /**
        * @brief    Entity�� ��ϵǾ��ִ� Transform ������Ʈ�� ��ȯ�մϴ�.
        * @return   Entity�� Transform ������Ʈ
        */
        Transform* GetTransform( ) { return m_transform; }

        /**
        * @brief    Entity�� �����Ǿ��ִ� �̸��� ��ȯ�մϴ�.
        * @return   Entity�� �̸�
        */
        std::string GetName( ) const { return m_name; }

        /**
        * @brief    Entity�� ���ο� �̸��� �������ݴϴ�.
        * @param    Entity�� �������� ���ο� �̸�
        */
        void SetName( const std::string& name ) { m_name = name; }

        
        void Start( );
        void Update( );
        void OnEnable( );
        void OnDisable( );

    protected:
        Context*            m_context;
        bool                m_bIsActive;
        Transform*          m_transform;

    private:
        Array<Component*>   m_components;
        std::string         m_name;

    };

    template <typename Ty>
    Ty* Entity::AddComponent( )
    {
        Component* component = new Ty( m_context );
        m_components.PushBack( component );

        component->Reset( );
        component->SetActive( true );
        component->m_transform = m_transform;

        return static_cast< Ty* >( component );
    }

    template <typename Ty>
    void Entity::RemoveComponent( Ty* target )
    {
        if ( target != nullptr )
        {
            for ( auto itr = m_components.begin( ); itr != m_components.end( ); ++itr )
            {
                m_context->GetAllocator( ).DeleteObject( target );
                m_components.Erase( itr );
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
                m_context->GetAllocator( ).DeleteObject( foundComponent );
                m_components.Erase( itr );
            }
        }
    }

    template <typename Ty>
    Ty* Entity::GetComponent( )
    {
        auto typeID = typeid( Ty );

        if ( m_transform != nullptr )
        {
            if ( typeID == typeid( *m_transform ) )
            {
                return m_transform;
            }
        }

        for ( auto component : m_components )
        {
            if ( typeID == typeid( *component ) )
            {
                return component;
            }
        }

        return nullptr;
    }

    template <typename Ty>
    Array<Ty*> Entity::GetComponents( )
    {
        auto typeID = typeid( Ty );
        Array<Ty*> tempArr{ m_context->GetAllocator( ) };

        if ( m_transform != nullptr )
        {
            if ( typeID == typeid( *m_transform ) )
            {
                tempArr.PushBack( m_transform );
            }

            return std::move( tempArr );
        }

        for ( auto component : m_components )
        {
            if ( typeID == typeid( *component ) )
            {
                tempArr.PushBack( component );
            }
        }

        return std::move( tempArr );
    }

    template <typename Ty>
    bool Entity::HasComponent( ) const
    {
        return ( ( GetComponent( ) ) != nullptr );
    }
}