#include "ResourceManager.h"
#include "ResourceCache.h"
#include "Core/Context.h"
#include "Core/Logger.h"

namespace Mile
{
   ResourceManager::ResourceManager( Context* context ) :
      SubSystem( context )
   {
   }

   ResourceManager::~ResourceManager( )
   {
      ClearCache( );
   }

   bool ResourceManager::Init( )
   {
      m_cache = std::make_unique<ResourceCache>( m_context );
      if ( m_cache == nullptr )
      {
         return false;
      }

      MELog( m_context, TEXT( "ResourceManager" ), ELogType::MESSAGE, TEXT( "Resource Manager Initialized!" ), true );
      return true;
   }

   void ResourceManager::ClearCache( )
   {
      m_cache->Clear( );
   }

}