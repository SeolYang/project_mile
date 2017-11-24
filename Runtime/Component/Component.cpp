#include "Component.h"

namespace Mile
{
    Component::Component( Entity* entity ) :
        m_entity( entity ),
        m_bIsActive( false )
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
}
