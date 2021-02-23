#pragma once
#include "Component/ComponentRegister.h"
#include "Core/Logger.h"

namespace Mile
{
   DECLARE_LOG_CATEGORY_EXTERN(MileSkyLightComponent, Log);

   class Texture2D;
   class MEAPI SkyLightComponent : public Component
   {
   public:
      DeclareComponent(SkyLightComponent);
      SkyLightComponent(Entity* entity);

      void OnCreate() override { SetTexture(nullptr); }

      json Serialize() const override;
      void DeSerialize(const json& jsonData) override;

      void SetTexture(Texture2D* texture);
      void SetTexture(const String& resourcePath);
      Texture2D* GetTexture() const { return m_skybox; }

      bool IsComputeAsRealtime() const { return m_bRealtime; }
      bool& Realtime() { return m_bRealtime; }

      float GetLuminanceMultiplier() const { return m_luminanceMultiplier; }
      float& LuminanceMultiplier() { return m_luminanceMultiplier; }

      void OnGUI() override;

   private:
      Texture2D* m_skybox;
      float m_luminanceMultiplier;
      bool m_bRealtime;

   };
}