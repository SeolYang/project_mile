#include "Resource.h"
#include "Core\Context.h"
namespace Mile
{
   Resource::Resource( Context* context, const String& path, ResourceType resourceType ) :
      m_context( context ),
      m_path( path ),
      m_name( GetFileNameFromPath( path ) ),
      m_folder( GetFolderFromPath( path ) ),
      m_resourceType( resourceType )
   {
   }


   String Resource::GetFileNameFromPath( const String& filePath )
   {
      auto splitPath = SplitStr( filePath, '/' );
      return splitPath[ splitPath.size( ) - 1 ];
   }

   String Resource::GetFolderFromPath( const String& filePath )
   {
      auto splitPath = SplitStr( filePath, '/' );
      splitPath.pop_back( );

      return CombineStr( splitPath, TEXT( "/" ) );
   }
}