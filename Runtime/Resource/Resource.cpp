#include "Resource.h"

namespace Mile
{
    Resource::Resource( Context* context, const std::string& path, const std::string& name, const std::string& folder, ResourceType resourceType ) :
        m_context( context ),
        m_path( path ),
        m_name( name ),
        m_folder( folder ),
        m_resourceType( resourceType )
    {
    }


    std::string Resource::GetFileNameFromPath( Allocator& allocator, const std::string& filePath )
    {
        auto splitPath = SplitStr( allocator, filePath, '\\' );
        auto splitFileName = SplitStr( allocator, splitPath[ splitPath.GetSize( ) - 1 ], '.' );
        splitFileName.PopBack( );

        return CombineStr( splitFileName, "." );
    }

    std::string Resource::GetFolderFromPath( Allocator& allocator, const std::string& filePath )
    {
        auto splitPath = SplitStr( allocator, filePath, '\\' );
        splitPath.PopBack( );

        return CombineStr( splitPath, "\\" );
    }
}

