#pragma once
#include "Component/ComponentRegister.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

namespace Mile
{
   enum class MEAPI ELightType : UINT32
   {
      Directional,
      Point,
   };

   class MEAPI LightComponent : public Component
   {
      DeclareComponent(LightComponent);

   public:
      LightComponent(Entity* entity);

      ELightType GetLightType() const { return m_type; }
      void SetLightType(ELightType type) { m_type = type; }

      Vector3 GetColor() const { return m_color; }
      void SetColor(const Vector3& newColor);

      float GetIntensity() const { return m_intensity; }
      float& Intensity() { return m_intensity; }

      Vector3 GetLightDirection() const;
      Vector3 GetLightPosition() const;

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

      void OnGUI() override;

   private:
      ELightType	m_type;
      Vector3	   m_color;
      float       m_intensity;

   };
}
