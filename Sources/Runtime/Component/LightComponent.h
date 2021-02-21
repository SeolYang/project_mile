#pragma once
#include "Component/ComponentRegister.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Rendering/Light.h"

namespace Mile
{
   /**
   * @brief 라이트 컴포넌트는 사용되는 타입에 따라 빛의 세기를 표현하는데 정해진 단위를 따른다
   * @Directional Light Lux(lx, lm/m^2)
   * @Point Light/Spot Light  Lumen(lm)
   */
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

      void OnGUI() override;

   private:
      ELightType	m_type;
      Vector3	   m_color;
      float       m_intensity;

   };
}
