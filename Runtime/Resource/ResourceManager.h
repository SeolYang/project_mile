#pragma once

#include "Resource.h"
#include "ResourceCache.h"

namespace Mile
{
    class MEAPI ResourceManager : public SubSystem
    {
    public:
        ResourceManager( Context* context );
        ~ResourceManager( );

        virtual bool Initialize( ) override;

        template < typename Ty >
        std::weak_ptr<Ty> Load( const std::string& relativePath )
        {
            if ( m_cache->HasByPath( relativePath ) )
            {
                return GetByPath( relativePath );
            }

            auto newResource = std::make_shared<Ty>( relativePath,
                                               Resource::GetFileNameFromPath( relativePath ),
                                               Resource::GetFolderFromPath( relativePath ) );

            if ( newResource->Initialize( ) )
            {
                m_cache->Add( std::dynamic_pointer_cast< Resource >( newResource ) );
            }

            return GetByPath<Ty>( relativePath );
        }

        template < typename Ty >
        std::weak_ptr<Ty> GetByPath( const std::string& filePath )
        {
            if ( m_cache->HasByPath( relativePath ) )
            {
                return std::dynamic_pointer_cast< Ty >( m_cache->GetByPath( filePath ) );
            }

            return std::weak_ptr<Ty>( );
        }

        template < typename Ty >
        std::weak_ptr<Ty> GetByName( const std::string& name )
        {
            if ( m_cache->HasByName( relativePath ) )
            {
                return std::dynamic_pointer_cast< Ty >( m_cache->GetByName( name ) );
            }

            return std::weak_ptr<Ty>( );
        }

        void ClearCache( );


    private:
        ResourceCachePtr    m_cache;

    };
}