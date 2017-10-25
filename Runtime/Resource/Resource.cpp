#include "Resource.h"
#include "Core\Context.h"
namespace Mile
{
    Resource::Resource( const std::string& path, ResourceType resourceType ) :
        m_path( path ),
        m_name( GetFileNameFromPath( path ) ),
        m_folder( GetFolderFromPath( path ) ),
        m_resourceType( resourceType )
    {
    }


    std::string Resource::GetFileNameFromPath( const std::string& filePath )
    {
        auto splitPath = SplitStr( filePath, '\\' );
        auto splitFileName = SplitStr( splitPath[ splitPath.size( ) - 1 ], '.' );
        splitFileName.pop_back( );

        return CombineStr( splitFileName, "." );
    }

    std::string Resource::GetFolderFromPath( const std::string& filePath )
    {
        auto splitPath = SplitStr( filePath, '\\' );
        splitPath.pop_back( );

        return CombineStr( splitPath, "\\" );
    }
}

