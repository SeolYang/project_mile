#pragma once
#include "SubSystem.h"
#include "json.hpp"
#include <tuple>

using json = nlohmann::json;

namespace Mile
{
   using Config = std::pair<String, json >;

   // Engine Config file must be Engine.json
   class MEAPI ConfigSystem : public SubSystem
   {
   public:
      ConfigSystem( Context* context );
      ~ConfigSystem( );

      virtual bool Init( ) override;

      bool IsExist( const String& configName ) const;

      bool LoadConfig( const String& configName );
      bool UnloadConfig( const String& configName );
      void UnloadAllConfigs( );

      bool SaveConfig( const String& configName );
      void SaveAllConfigs( );

      Config& GetConfig( const String& configName );
      Config GetConfig( const String& configName ) const;

   private:
      static String GetPathFromName( const String& configName )
      {
         return ( TEXT( "Content/Configs/" ) + configName + TEXT( ".json" ) );
      }

   private:
      std::vector<Config> m_configs;
      Config              m_nullConfig;

   };
}