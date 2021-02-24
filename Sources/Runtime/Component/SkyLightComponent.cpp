#include "Component/SkyLightComponent.h"
#include "Core/Engine.h"
#include "Core/ImGuiHelper.h"
#include "Resource/ResourceManager.h"
#include "Resource/Texture2D.h"

namespace Mile
{
   DefineComponent(SkyLightComponent);
   DEFINE_LOG_CATEGORY(MileSkyLightComponent);

   SkyLightComponent::SkyLightComponent(Entity* entity) :
      Component(entity),
      m_intensityScale(1.0f),
      m_bRealtime(false),
      m_skybox(nullptr)
   {
   }

   json SkyLightComponent::Serialize() const
   {
      json serialized = Component::Serialize();
      serialized["Path"] = WString2String(m_skybox->GetPath());
      serialized["RealtimeCapture"] = m_bRealtime;
      serialized["IntensityScale"] = m_intensityScale;

      return serialized;
   }

   void SkyLightComponent::DeSerialize(const json& jsonData)
   {
      Component::DeSerialize(jsonData);
      SetTexture(String2WString(GetValueSafelyFromJson(jsonData, "Path", std::string())));
      m_bRealtime = GetValueSafelyFromJson(jsonData, "RealtimeCapture", false);
      m_intensityScale = GetValueSafelyFromJson(jsonData, "IntensityScale", 1.0f);
   }

   void SkyLightComponent::SetTexture(Texture2D* texture)
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

   void SkyLightComponent::SetTexture(const String& resourcePath)
   {
      auto texture = Engine::GetResourceManager()->Load<Texture2D>(resourcePath);
      if (texture == nullptr)
      {
         ME_LOG(MileSkyLightComponent, Warning, (TEXT("Failed to load skybox texture from : ") + resourcePath));
      }

      SetTexture(texture);
   }

   void SkyLightComponent::OnGUI()
   {
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Intensity Scale");
      ImGui::TableSetColumnIndex(1);
      ImGui::InputFloat("##Intensity Scale", &m_intensityScale);

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Real Time Capture");
      ImGui::TableSetColumnIndex(1);
      ImGui::Checkbox("##RealtimeCapture", &m_bRealtime);
   }
}