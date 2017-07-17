#include "ResourceCache.h"
#include "Core/Context.h"

namespace Mile
{
    ResourceCache::ResourceCache( Context* context ) :
        m_context( context ),
        m_resources( context->GetAllocator( ) )
    {
    }

    ResourceCache::~ResourceCache( )
    {
        Clear( );
    }

    void ResourceCache::Add( ResourcePtr res )
    {
        if ( res != nullptr )
        {
            m_resources.PushBack( res );
        }
    }

    void ResourceCache::Clear( )
    {
        m_resources.Clear( );
        m_resources.Shrink( );
    }

    WeakResourcePtr ResourceCache::GetByPath( const std::string& path )
    {
        for ( auto res : m_resources )
        {
            if ( res->GetPath( ) == path )
            {
                return res;
            }
        }

        return WeakResourcePtr( );
    }

    WeakResourcePtr ResourceCache::GetByName( const std::string& name )
    {
        for ( auto res : m_resources )
        {
            if ( res->GetName( ) == name )
            {
                return res;
            }
        }

        return WeakResourcePtr( );
    }

    bool ResourceCache::CachedByPath( const std::string& name )
    {
        for ( auto res : m_resources )
        {
            if ( res->GetPath( ) == name )
            {
                return true;
            }
        }

        return false;
    }

    bool ResourceCache::CachedByName( const std::string& name )
    {
        for ( auto res : m_resources )
        {
            if ( res->GetName( ) == name )
            {
                return true;
            }
        }

        return false;
    }

}