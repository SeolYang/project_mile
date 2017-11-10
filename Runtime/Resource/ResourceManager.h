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

        virtual bool Init( ) override;

        template < typename Ty >
        std::weak_ptr<Ty> Load( const String& relativePath )
        {
            if ( m_cache->HasByPath( relativePath ) )
            {
                return GetByPath<Ty>( relativePath );
            }

            auto newResource = std::make_shared<Ty>( m_context, relativePath );
            if ( newResource->Init( ) )
            {
                m_cache->Add( std::dynamic_pointer_cast< Resource >( newResource ) );
                return newResource;
            }

            return std::weak_ptr<Ty>( );
        }

        template < typename Ty >
        std::weak_ptr<Ty> GetByPath( const String& filePath )
        {
            if ( m_cache->HasByPath( filePath ) )
            {
                return std::dynamic_pointer_cast< Ty >( m_cache->GetByPath( filePath ) );
            }

            return std::weak_ptr<Ty>( );
        }

        template < typename Ty >
        std::weak_ptr<Ty> GetByName( const String& name )
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