#pragma once
#include "Component/ComponentRegister.h"
#include "Core/Logger.h"

namespace Mile
{
   DECLARE_LOG_CATEGORY_EXTERN(MileSkyboxComponent, Log);

   class Texture2D;
   class MEAPI SkyboxComponent : public Component
   {
   public:
      DeclareComponent(SkyboxComponent);
      SkyboxComponent(Entity* entity);

      json Serialize() const override;
      void DeSerialize(const json& jsonData) override;

      void SetTexture(Texture2D* texture);
      void SetTexture(const String& resourcePath);
      Texture2D* GetTexture() const { return m_skybox; }

   private:
      Texture2D* m_skybox;

   };
}