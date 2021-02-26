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

   /** 
   * @params   aperture       f-stops
   * @params   shutterSpeed   seconds
   * @params   sensitivity    ISO
   */
   inline static float EV100(float aperture, float shutterSpeed, float sensitivity)
   {
      // EV = log_2(N^2/t) + log_2(100.0f/S)
      // EV_100 = log_2(N^2/t)
      // EC(Exposure Compensation) = log_2(Sensitivity/100.0f);
      // EV_100' = EV_100 - EC = log_2(N^2/t) - log_2(Sensitivity/100.0f) = log_2((N^2/t) / (Sensitivity/100.0f)) = log_2((N^2/t) * (100.0f/Sensitivity))
      float ev = (aperture * aperture) / shutterSpeed;
      float ec = 100.0f / sensitivity;
      return std::log2(ev * ec);
   }

   /** Saturation based speed relation */
   inline static float ExposureNormalizationFactor(float ev100)
   {
      return 1.0f / (std::pow(2.0f, ev100) * 1.2f);
   }
}