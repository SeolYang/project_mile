#pragma once

#include "Core/Helper.h"
#include "Resource.h"

namespace Mile
{
    class MEAPI ResourceCache
    {
    public:
        ResourceCache( Context* context );
        ~ResourceCache( );

        void Add( ResourcePtr res );
        void Clear( );

        WeakResourcePtr GetByPath( const std::string& path );
        WeakResourcePtr GetByName( const std::string& name );
        bool CachedByPath( const std::string& path );
        bool CachedByName( const std::string& name );

    private:
        Context*                m_context;
        Array<ResourcePtr>      m_resources;

    };

    using ResourceCachePtr = std::unique_ptr<ResourceCache, std::function<void(ResourceCache*)>>;
}