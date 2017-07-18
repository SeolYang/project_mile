#include "ResourceManager.h"
#include "ResourceCache.h"
#include "Core/Context.h"

namespace Mile
{
    ResourceManager::ResourceManager( Context* context ) :
        SubSystem( context )
    {
    }

    ResourceManager::~ResourceManager( )
    {
    }

    bool ResourceManager::Initialize( )
    {
        m_cache = std::move( MakeUnique<ResourceCache>( m_context->GetAllocator( ), m_context ) );
        if ( m_cache != nullptr )
        {
            return true;
        }

        return false;
    }

    void ResourceManager::ClearCache( )
    {
        m_cache->Clear( );
    }

}