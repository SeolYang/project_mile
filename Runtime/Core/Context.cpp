#include "Context.h"

namespace Mile
{
    Context::Context( Allocator& allocator ) :
        m_allocator( allocator ),
        m_subSystems( allocator )
    {

    }

    Context::~Context( )
    {
        for ( size_t idx = 1; idx < m_subSystems.GetSize( ) - 1; ++idx )
        {
            RUMIA_DELETE( m_allocator, m_subSystems[ idx ] );
        }
    }

    void Context::RegisterSubSystem( SubSystem* newSubSystem )
    {
        if ( newSubSystem != nullptr )
        {
            m_subSystems.PushBack( newSubSystem );
        }
    }
}