#include "Component/SkyboxComponent.h"
#include "Core/Engine.h"
#include "Resource/ResourceManager.h"
#include "Resource/Texture2D.h"

namespace Mile
{
   DefineComponent(SkyboxComponent);
   DEFINE_LOG_CATEGORY(MileSkyboxComponent);

   SkyboxComponent::SkyboxComponent(Entity* entity) :
      Component(entity),
      m_skybox(nullptr)
   {
      SetTexture(nullptr);
   }

   json SkyboxComponent::Serialize() const
   {
      json serialized = Component::Serialize();
      serialized["Path"] = WString2String(m_skybox->GetPath());
      return serialized;
   }

   void SkyboxComponent::DeSerialize(const json& jsonData)
   {
      Component::DeSerialize(jsonData);
      SetTexture(String2WString(GetValueSafelyFromJson(jsonData, "Path", std::string())));
   }

   void SkyboxComponent::SetTexture(Texture2D* texture)
   {
      if (texture == nullptr)
      {
         m_skybox = Engine::GetResourceManager()->GetByPath<Texture2D>(TEXT("Contents/Textures/default_black.png"));
      }
      else
      {
         m_skybox = texture;
      }
   }

   void SkyboxComponent::SetTexture(const String& resourcePath)
   {
      auto texture = Engine::GetResourceManager()->GetByPath<Texture2D>(resourcePath);
      if (texture == nullptr)
      {
         ME_LOG(MileSkyboxComponent, Warning, (TEXT("Failed to load skybox texture from : ") + resourcePath));
      }

      SetTexture(texture);
   }
}