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

    ResourcePtr ResourceCache::GetByPath( const std::string& path )
    {
        for ( auto res : m_resources )
        {
            if ( res->GetPath( ) == path )
            {
                return res;
            }
        }

        return ResourcePtr( );
    }

    ResourcePtr ResourceCache::GetByName( const std::string& name )
    {
        for ( auto res : m_resources )
        {
            if ( res->GetName( ) == name )
            {
                return res;
            }
        }

        return ResourcePtr( );
    }

    bool ResourceCache::HasByPath( const std::string& name )
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

    bool ResourceCache::HasByName( const std::string& name )
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