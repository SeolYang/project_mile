#pragma once
#include "Component/ComponentRegister.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

namespace Mile
{
   enum class ELightType : UINT32
   {
      Directional,
      Point,
   };

   class MEAPI LightComponent : public Component
   {
      ComponentBegin(LightComponent)

   public:
      LightComponent(Entity* entity);

      ELightType GetLightType() const { return m_type; }
      void SetLightType(ELightType type) { m_type = type; }

      Vector3 GetRadiance() const { return m_radiance; }
      void SetRadiance(const Vector3& radiance) { m_radiance = radiance; }

      Vector3 GetLightDirection() const;
      Vector3 GetLightPosition() const;

      //virtual std::string Serialize( ) const override;
      virtual json Serialize() const override;
      virtual void DeSerialize(const json& jsonData) override;

      static std::string LightTypeToString(ELightType type)
      {
         switch (type)
         {
         case ELightType::Directional:
            return "Directional";
         case ELightType::Point:
            return "Point";
         }

         return "Unknown";
      }

      static ELightType StringToLightType(const std::string& str)
      {
         if (str == "Point")
         {
            return ELightType::Point;
         }

         return ELightType::Directional;
      }

      static unsigned int LightTypeToIndex(ELightType type)
      {
         switch (type)
         {
         case ELightType::Directional:
            return 0;
         case ELightType::Point:
            return 1;
         }

         return 0;
      }

   private:
      ELightType	m_type;
      Vector3	   m_radiance;

   };
}
