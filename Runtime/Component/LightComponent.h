#pragma once

#include "Component.h"
#include "Core/ComponentRegister.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

namespace Mile
{
   enum class LightType
   {
      Directional,
      Point,
      Spot
   };

   class MEAPI LightComponent : public Component
   {
      ComponentBegin( LightComponent )

   public:
      LightComponent( Entity* entity );

      LightType GetLightType( ) const { return m_type; }
      void SetLightType( LightType type ) { m_type = type; }

      Vector3 GetLightColor( ) const { return m_color; }
      void SetLightColor( const Vector3& color ) { m_color = color; }

      Vector2 GetSpotlightAngles( ) const { return m_spotlightAngles; }
      void SetSpotlightAngles( const Vector2& angles ) { m_spotlightAngles = angles; }

      float GetLightRange( ) const { return m_lightRange; }
      void SetLightRange( float range ) { m_lightRange = range; }

      Vector3 GetLightDirection( ) const;
      Vector3 GetLightPosition( ) const;

      virtual std::string Serialize( ) const override;
      virtual void DeSerialize( const json& jsonData ) override;

      static std::string LightTypeToString( LightType type )
      {
         switch ( type )
         {
         case LightType::Directional:
            return "Directional";
         case LightType::Point:
            return "Point";
         case LightType::Spot:
            return "Spot";
         }

         return "Unknown";
      }
      static LightType StringToLightType( const std::string& str )
      {
         if ( str == "Spot" )
         {
            return LightType::Spot;
         }
         else if ( str == "Point" )
         {
            return LightType::Point;
         }

         return LightType::Directional;
      }
      static unsigned int LightTypeToIndex( LightType type )
      {
         switch ( type )
         {
         case LightType::Directional:
            return 0;
         case LightType::Point:
            return 1;
         case LightType::Spot:
            return 2;
         }

         return 0;
      }

   private:
      LightType      m_type;
      Vector3        m_color;
      Vector2        m_spotlightAngles;
      float          m_lightRange;

   };
}
