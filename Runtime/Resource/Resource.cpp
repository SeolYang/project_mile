#include "Resource.h"
#include "Core\Context.h"
namespace Mile
{
    Resource::Resource( const String& path, ResourceType resourceType ) :
        m_path( path ),
        m_name( GetFileNameFromPath( path ) ),
        m_folder( GetFolderFromPath( path ) ),
        m_resourceType( resourceType )
    {
    }


    String Resource::GetFileNameFromPath( const String& filePath )
    {
        auto splitPath = SplitStr( filePath, '\\' );
        auto splitFileName = SplitStr( splitPath[ splitPath.size( ) - 1 ], '.' );
        splitFileName.pop_back( );

        return CombineStr( splitFileName, TEXT("."));
    }

    String Resource::GetFolderFromPath( const String& filePath )
    {
        auto splitPath = SplitStr( filePath, '\\' );
        splitPath.pop_back( );

        return CombineStr( splitPath, TEXT( "\\" ) );
    }
}