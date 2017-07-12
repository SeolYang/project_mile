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

        void Initialize( Transform* transform );

        template <typename Ty> Ty* AddComponent( );
        template <typename Ty> void RemoveComponent( Ty* target );
        template <typename Ty> void RemoveComponents( );
        template <typename Ty> Ty* GetComponent( );
        template <typename Ty> Array<Ty*> GetComponents( );
        template <typename Ty> bool HasComponent( ) const;

        bool IsActive( ) const { return m_bIsActive; }
        void SetActive( bool bIsActive );

        Transform* GetTransform( ) { return m_transform; }

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
