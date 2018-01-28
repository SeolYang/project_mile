#include "Component.h"
#include "Core\Entity.h"

namespace Mile
{
    Component::Component( Entity* entity ) :
        m_entity( entity ),
        m_bIsActive( true )
    {

    }

    void Component::SetActive( bool bIsActive )
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

    Context* Component::GetContext( ) const
    {
       return m_entity->GetContext( );
    }

    Transform* Component::GetTransform( ) const
    {
       return m_entity->GetTransform( );
    }
}
