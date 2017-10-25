#include "Resource.h"
#include "Core\Context.h"
namespace Mile
{
    Resource::Resource( Context* context, const std::string& path, ResourceType resourceType ) :
        m_context( context ),
        m_path( path ),
        m_name( GetFileNameFromPath( context, path ) ),
        m_folder( GetFolderFromPath( context, path ) ),
        m_resourceType( resourceType )
    {
    }


    std::string Resource::GetFileNameFromPath( Context* context, const std::string& filePath )
    {
       Allocator& allocator = context->GetAllocator( );
        auto splitPath = SplitStr( allocator, filePath, '\\' );
        auto splitFileName = SplitStr( allocator, splitPath[ splitPath.GetSize( ) - 1 ], '.' );
        splitFileName.PopBack( );

        return CombineStr( splitFileName, "." );
    }

    std::string Resource::GetFolderFromPath( Context* context, const std::string& filePath )
    {
       Allocator& allocator = context->GetAllocator( );
        auto splitPath = SplitStr( allocator, filePath, '\\' );
        splitPath.PopBack( );

        return CombineStr( splitPath, "\\" );
    }
}

