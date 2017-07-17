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

    std::string ResourceManager::GetFileNameFromPath( Allocator& allocator, const std::string& filePath )
    {
        auto splitPath = SplitStr( allocator, filePath, '\\' );
        auto splitFileName = SplitStr( allocator, splitPath[ splitPath.GetSize( ) - 1 ], '.' );
        splitFileName.PopBack( );

        return CombineStr( splitFileName, "." );
    }

    std::string ResourceManager::GetFolderFromPath( Allocator& allocator, const std::string& filePath )
    {
        auto splitPath = SplitStr( allocator, filePath, '\\' );
        splitPath.PopBack( );

        return CombineStr( splitPath, "\\" );
    }

}