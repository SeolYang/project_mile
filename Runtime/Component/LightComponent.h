#pragma once
#include "Component/Component.h"
#include "Component/ComponentRegister.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

namespace Mile
{
   enum class LightType
   {
      Directional,
      Point,
   };

   class MEAPI LightComponent : public Component
   {
      ComponentBegin(LightComponent)

   public:
      LightComponent(Entity* entity);

      LightType GetLightType() const { return m_type; }
      void SetLightType(LightType type) { m_type = type; }

      Vector3 GetRadiance() const { return m_radiance; }
      void SetRadiance(const Vector3& radiance) { m_radiance = radiance; }

      Vector3 GetLightDirection() const;
      Vector3 GetLightPosition() const;

      //virtual std::string Serialize( ) const override;
      virtual json Serialize() const override;
      virtual void DeSerialize(const json& jsonData) override;

      static std::string LightTypeToString(LightType type)
      {
         switch (type)
         {
         case LightType::Directional:
            return "Directional";
         case LightType::Point:
            return "Point";
         }

         return "Unknown";
      }

      static LightType StringToLightType(const std::string& str)
      {
         if (str == "Point")
         {
            return LightType::Point;
         }

         return LightType::Directional;
      }

      static unsigned int LightTypeToIndex(LightType type)
      {
         switch (type)
         {
         case LightType::Directional:
            return 0;
         case LightType::Point:
            return 1;
         }

         return 0;
      }

   private:
      LightType	m_type;
      Vector3	m_radiance;

   };
}
