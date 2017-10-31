#include "ResourceCache.h"
#include "Core/Context.h"

namespace Mile
{
    ResourceCache::ResourceCache( Context* context ) :
        m_context( context )
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
            m_resources.push_back( res );
        }
    }

    void ResourceCache::Clear( )
    {
        m_resources.clear( );
        m_resources.shrink_to_fit( );
    }

    ResourcePtr ResourceCache::GetByPath( const String& path )
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

    ResourcePtr ResourceCache::GetByName( const String& name )
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

    bool ResourceCache::HasByPath( const String& name )
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

    bool ResourceCache::HasByName( const String& name )
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