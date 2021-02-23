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

      bool IsRealtimeCapture() const { return m_bRealtime; }
      bool& IsRealtimeCapture() { return m_bRealtime; }

      float IntensityScale() const { return m_intensityScale; }
      float& InensityScale() { return m_intensityScale; }

      void OnGUI() override;

   private:
      Texture2D* m_skybox;
      float m_intensityScale;
      bool m_bRealtime;

   };
}