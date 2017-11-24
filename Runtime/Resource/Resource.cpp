#include "Resource.h"
#include "Core\Context.h"
#include <cwctype>

namespace Mile
{
   Resource::Resource( Context* context, const String& path, ResourceType resourceType ) :
      m_context( context ),
      m_path( path ),
      m_name( GetFileNameFromPath( path ) ),
      m_ext( GetFileExtensionFromPath( m_path ) ),
      m_folder( GetFolderFromPath( path ) ),
      m_resourceType( resourceType )
   {
      std::transform( m_ext.begin( ), m_ext.end( ), m_ext.begin( ), ::towlower );
   }

   String Resource::GetFileNameFromPath( const String& filePath )
   {
      auto splitPath = SplitStr( filePath, '/' );
      auto splitFileName = SplitStr( splitPath[ splitPath.size( ) - 1 ], '.' );
      return splitFileName[ 0 ];
   }

   String Resource::GetFolderFromPath( const String& filePath )
   {
      auto splitPath = SplitStr( filePath, '/' );
      splitPath.pop_back( );

      return CombineStr( splitPath, TEXT( "/" ) );
   }

   String Resource::GetFileExtensionFromPath( const String& filePath )
   {
      auto name = GetFileNameFromPath( filePath );
      auto splitName = SplitStr( name, '.' );
      return splitName[ splitName.size( ) - 1 ];
   }
}