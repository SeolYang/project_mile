#include "Config.h"
#include "Context.h"
#include "Resource\ResourceManager.h"
#include "Resource\PlainText.h"
#include "Core\Logger.h"

namespace Mile
{
   ConfigSystem::ConfigSystem( Context* context ) : SubSystem( context ),
      m_nullConfig( TEXT( "NULL" ), json( ) )
   {
   }

   ConfigSystem::~ConfigSystem( )
   {
      UnloadAllConfigs( );
   }

   bool ConfigSystem::Init( )
   {
      if ( !LoadConfig( TEXT("Engine" )) )
      {
         return false;
      }

      MELog( m_context, TEXT( "ConfigSystem" ), ELogType::MESSAGE, TEXT( "Config System Initialized!" ), true );
      return true;
   }

   bool ConfigSystem::IsExist( const String & configName ) const
   {
      for ( auto& config : m_configs )
      {
         if ( config.first == configName )
         {
            return true;
         }
      }

      return false;
   }

   bool ConfigSystem::LoadConfig( const String& configName )
   {
      if ( !IsExist( configName ) )
      {
         auto resManager = m_context->GetSubSystem<ResourceManager>( );
         auto text = resManager->Load<PlainText<>>( GetPathFromName( configName ) );
         
         if ( text != nullptr )
         {
            m_configs.push_back( std::make_pair(
               configName,
               json::parse( WString2String( text->GetData( ) ) ) ) );

            return true;
         }
      }

      // Failed to load engine config!
      return false;
   }

   bool ConfigSystem::UnloadConfig( const String& configName )
   {
      for ( auto itr = m_configs.begin( ); itr != m_configs.end( ); ++itr )
      {
         if ( ( *itr ).first == configName )
         {
            m_configs.erase( itr );
            return true;
         }
      }

      return false;
   }

   void ConfigSystem::UnloadAllConfigs( )
   {
      m_configs.clear( );
      m_configs.shrink_to_fit( );
   }

   bool ConfigSystem::SaveConfig( const String& configName )
   {
      if ( IsExist( configName ) )
      {
         Config& config = GetConfig( configName );
         auto resManager = m_context->GetSubSystem<ResourceManager>( );
         auto plainText = resManager->Load<PlainText<>>( GetPathFromName( configName ) );
         auto dumpData = config.second.dump( );
         plainText->SetData( String2WString( dumpData ) );
         return plainText->Save( );
      }

      return false;
   }

   void ConfigSystem::SaveAllConfigs( )
   {
      auto resManager = m_context->GetSubSystem<ResourceManager>( );
      for ( auto& config : m_configs )
      {
         auto plainText = resManager->Load<PlainText<>>( GetPathFromName( config.first ) );
         auto dumpData = config.second.dump( );
         plainText->SetData( String2WString( dumpData ) );
         plainText->Save( );
      }
   }

   Config& ConfigSystem::GetConfig( const String& configName )
   {
      for ( auto& config : m_configs )
      {
         if ( config.first == configName )
         {
            return config;
         }
      }

      return m_nullConfig;
   }

   Config ConfigSystem::GetConfig( const String& configName ) const
   {
      for ( auto& config : m_configs )
      {
         if ( config.first == configName )
         {
            return config;
         }
      }

      return m_nullConfig;
   }
}