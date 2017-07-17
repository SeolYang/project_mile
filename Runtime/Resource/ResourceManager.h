#pragma once

#include "Resource.h"
#include "ResourceCache.h"

namespace Mile
{
    class Setting;
    class MEAPI ResourceManager : public SubSystem
    {
    public:
        ResourceManager( Context* context );
        ~ResourceManager( );

        virtual bool Initialize( ) override;

        template < typename Ty >
        std::weak_ptr<Ty> Load( const std::string& relativePath )
        {
            //@TODO Implement Load
        }

        void ClearCache( );

        static std::string GetFileNameFromPath( Allocator& allocator, const std::string& filePath );
        static std::string GetFolderFromPath( Allocator& allocator, const std::string& filePath );

    private:
        ResourceCachePtr    m_cache;

    };
}