#include "Resource.h"

namespace Mile
{
    Resource::Resource( Context* context, const std::string& name, const std::string& path, ResourceType resourceType ) :
        m_context( context ),
        m_name( name ),
        m_path( path ),
        m_resourceType( resourceType )
    {
    }
}

