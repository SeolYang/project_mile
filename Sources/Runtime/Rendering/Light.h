#pragma once
#include "Core/CoreMinimal.h"
#include "Math/MathMinimal.h"

namespace Mile
{
   constexpr float MaximumPossibleLuminousEfficacy = 683.0f;

   enum class MEAPI ELightType : UINT32
   {
      Directional,
      Point,
      Spot
   };

   enum class MEAPI ELightIntensityUnit : UINT32
   {
      Lumen, /** lm */
      Candela, /** cd (lm/sr) */
      Lux, /** lx (lm/m^2) */
      Nit /** nt (cd/m^2) */
   };

   inline static ELightIntensityUnit LightIntensityUnitOf(const ELightType& lightType)
   {
      switch (lightType)
      {
      case ELightType::Directional:
         return ELightIntensityUnit::Lux;
      default:
      case ELightType::Point:
      case ELightType::Spot:
         return ELightIntensityUnit::Lumen;
      }
   }

   inline static std::string LightIntensityUnitToString(const ELightIntensityUnit& unit, bool bAsSymbol = false)
   {
      if (bAsSymbol)
      {
         switch (unit)
         {
         case ELightIntensityUnit::Lumen:
            return std::string("lm");
         case ELightIntensityUnit::Candela:
            return std::string("cd");
         case ELightIntensityUnit::Lux:
            return std::string("lx");
         case ELightIntensityUnit::Nit:
            return std::string("nt");
         }
      }
      else
      {
         switch (unit)
         {
         case ELightIntensityUnit::Lumen:
            return std::string("Lumen");
         case ELightIntensityUnit::Candela:
            return std::string("Candela");
         case ELightIntensityUnit::Lux:
            return std::string("Lux");
         case ELightIntensityUnit::Nit:
            return std::string("Nit");
         }
      }

      return std::string();
   }

   inline static float WattsToLumens(float Watt, float efficiency)
   {
      return Watt * (MaximumPossibleLuminousEfficacy * efficiency);
   }
}