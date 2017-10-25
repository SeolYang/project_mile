#include "Entity.h"
#include "Context.h"
#include "Component\Component.h"

namespace Mile
{
    Entity::Entity( Context* context ) :
        m_context( context ),
        m_bIsActive( false ),
        m_transform( nullptr )
    {
    }

    Entity::~Entity( )
    {
    }

    void Entity::Initialize( Transform* transform )
    {
        m_transform = transform;
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