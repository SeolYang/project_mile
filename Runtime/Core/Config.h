#pragma once
#include "SubSystem.h"
#include "json.hpp"
#include <tuple>

using json = nlohmann::json;

namespace Mile
{
   using Config = std::pair<std::string, json >;

   // Engine Config file must be Engine.json
   class MEAPI ConfigSystem : public SubSystem
   {
   public:
      ConfigSystem( Context* context );
      ~ConfigSystem( );

      virtual bool Initialize( ) override;

      bool IsExist( const std::string& configName ) const;

      bool LoadConfig( const std::string& configName );
      bool UnloadConfig( const std::string& configName );
      void UnloadAllConfigs( );

      bool SaveConfig( const std::string& configName );
      void SaveAllConfigs( );

      Config& GetConfig( const std::string& configName );
      Config GetConfig( const std::string& configName ) const;

   private:
      static std::string GetPathFromName( const std::string& configName )
      {
         return ( "Content/Configs/" + configName + ".json" );
      }

   private:
      std::vector<Config> m_configs;

   };
}