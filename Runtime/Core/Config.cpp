#include "Config.h"
#include "Context.h"
#include "Resource\ResourceManager.h"
#include "Resource\PlainText.h"

namespace Mile
{
   ConfigSystem::ConfigSystem( Context* context ) : SubSystem( context )
   {
   }

   ConfigSystem::~ConfigSystem( )
   {
      UnloadAllConfigs( );
   }

   bool ConfigSystem::Initialize( )
   {
      if ( !LoadConfig( "Engine" ) )
      {
         return false;
      }

      return true;
   }

   bool ConfigSystem::IsExist( const std::string & configName ) const
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

   bool ConfigSystem::LoadConfig( const std::string& configName )
   {
      if ( !IsExist( configName ) )
      {
         auto resManager = m_context->GetSubSystem<ResourceManager>( );
         auto text = resManager->Load<PlainText>( GetPathFromName( configName ) );
         m_configs.push_back( std::make_pair( 
            configName,
            json::parse( text._Get( )->GetData( ) ) ) );

         return true;
      }

      return false;
   }

   bool ConfigSystem::UnloadConfig( const std::string& configName )
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

   bool ConfigSystem::SaveConfig( const std::string& configName )
   {
      if ( IsExist( configName ) )
      {
         Config& config = GetConfig( configName );
         auto resManager = m_context->GetSubSystem<ResourceManager>( );
         auto plainText = resManager->Load<PlainText>( GetPathFromName( configName ) );
         auto plainTextRawPtr = plainText._Get( );
         plainTextRawPtr->SetData( config.second.dump( ) );
         return plainTextRawPtr->Save( );
      }

      return false;
   }

   void ConfigSystem::SaveAllConfigs( )
   {
      auto resManager = m_context->GetSubSystem<ResourceManager>( );
      for ( auto& config : m_configs )
      {
         auto plainText = resManager->Load<PlainText>( GetPathFromName( config.first ) );
         auto plainTextRawPtr = plainText._Get( );
         plainTextRawPtr->SetData( config.second.dump( ) );
         plainTextRawPtr->Save( );
      }
   }

   Config& ConfigSystem::GetConfig( const std::string& configName )
   {
      for ( auto& config : m_configs )
      {
         if ( config.first == configName )
         {
            return config;
         }
      }

      return Config( "NotFound", json( ) );
   }

   Config ConfigSystem::GetConfig( const std::string& configName ) const
   {
      for ( auto& config : m_configs )
      {
         if ( config.first == configName )
         {
            return config;
         }
      }

      return Config( "NotFound", json() );
   }
}