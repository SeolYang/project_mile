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

        ResourcePtr GetByPath( const std::string& path );
        ResourcePtr GetByName( const std::string& name );
        bool HasByPath( const std::string& path );
        bool HasByName( const std::string& name );

    private:
        Context*                      m_context;
        std::vector<ResourcePtr>      m_resources;

    };

    using ResourceCachePtr = std::unique_ptr<ResourceCache, std::function<void(ResourceCache*)>>;
}