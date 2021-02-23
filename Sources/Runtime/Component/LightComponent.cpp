#include "Component/LightComponent.h"
#include "GameFramework/Entity.h"
#include "GameFramework/Transform.h"
#include "Core/ImGuiHelper.h"

namespace Mile
{
   DefineComponent(LightComponent);

   LightComponent::LightComponent(Entity* entity) :
      m_type(ELightType::Directional),
      m_color(Vector3(1.0f, 1.0f, 1.0f)),
      m_intensity(1.0f),
      m_radius(1.0f),
      m_outerAngle(15.0f),
      Component(entity)
   {
      m_bCanEverUpdate = false;
   }

   void LightComponent::SetColor(const Vector3& newColor)
   {
      m_color.x = std::clamp(newColor.x, 0.0f, 1.0f);
      m_color.y = std::clamp(newColor.y, 0.0f, 1.0f);
      m_color.z = std::clamp(newColor.z, 0.0f, 1.0f);
   }

   Vector3 LightComponent::GetLightDirection() const
   {
      auto transform = m_entity->GetTransform();
      return transform->GetForward();
   }

   Vector3 LightComponent::GetLightPosition() const
   {
      auto transform = m_entity->GetTransform();
      return transform->GetPosition(ETransformSpace::World);
   }

   json LightComponent::Serialize() const
   {
      json serialized = Component::Serialize();
      serialized["LightType"] = static_cast<UINT32>(m_type);
      serialized["Color"] = m_color.Serialize();
      serialized["Intensity"] = m_intensity;
      serialized["Radius"] = m_radius;
      serialized["InnerAngle"] = m_innerAngle;
      serialized["OuterAngle"] = m_outerAngle;
      return serialized;
   }

   void LightComponent::DeSerialize(const json& jsonData)
   {
      Component::DeSerialize(jsonData);
      m_type = static_cast<ELightType>(GetValueSafelyFromJson<UINT32>(jsonData, "LightType", 0));

      Vector3 tempColor;
      tempColor.DeSerialize(jsonData["Color"]);
      SetColor(tempColor);

      m_intensity = GetValueSafelyFromJson<float>(jsonData, "Intensity", 1.0f);
      m_radius = GetValueSafelyFromJson<float>(jsonData, "Radius", 1.0f);

      SetOuterAngle(GetValueSafelyFromJson<float>(jsonData, "OuterAngle", 15.0f));
      SetInnerAngle(GetValueSafelyFromJson<float>(jsonData, "InnerAngle", 15.0f));
   }

   void LightComponent::OnGUI()
   {
      std::string intensityInputLabel = "Light Intensity (";
      intensityInputLabel.append(LightIntensityUnitToString(LightIntensityUnitOf(m_type)));
      intensityInputLabel.append(")");

      ImGui::Spacing();
      ImGui::Text("Light Type");
      ImGui::Spacing(); ImGui::Spacing();
      ImGui::Text("Light Color");
      ImGui::Spacing(); ImGui::Spacing();
      ImGui::Text(intensityInputLabel.c_str());
      ImGui::Spacing(); ImGui::Spacing();

      switch (m_type)
      {
      case Mile::ELightType::Spot:
         ImGui::Text("Outer Angle");
         ImGui::Spacing(); ImGui::Spacing();
         ImGui::Text("Inner Angle");
         ImGui::Spacing(); ImGui::Spacing();
      case Mile::ELightType::Point:
         ImGui::Text("Light Radius");
         break;
      }

      ImGui::NextColumn();

      const char* items[] = { "Directional", "Point", "Spot" };
      static const char* currentItem = items[static_cast<UINT32>(m_type)];
      ImGui::Spacing();
      if (ImGui::BeginCombo("##Type", currentItem))
      {
         for (UINT32 idx = 0; idx < IM_ARRAYSIZE(items); ++idx)
         {
            bool bSelected = (currentItem == items[idx]);
            if (ImGui::Selectable(items[idx], bSelected))
            {
               currentItem = items[idx];
               m_type = static_cast<ELightType>(idx);
            }
            if (bSelected)
            {
               ImGui::SetItemDefaultFocus();
            }
         }

         ImGui::EndCombo();
      }

      GUI::Vector3Input("##Color", m_color, 0.01f, 0.0f, 1.0f);

      GUI::FloatInput(("##" + intensityInputLabel).c_str(), m_intensity, 0.1f, 0.0f, 100000.0f, true);

      float tempAngle = m_outerAngle;
      switch (m_type)
      {
      case Mile::ELightType::Spot:
         if (ImGui::InputFloat("##Outer Angle", &tempAngle))
         {
            SetOuterAngle(tempAngle);
         }

         tempAngle = m_innerAngle;
         if (ImGui::InputFloat("##Inner Angle", &tempAngle))
         {
            SetInnerAngle(tempAngle);
         }
      case Mile::ELightType::Point:
         ImGui::InputFloat("##Radius", &m_radius);
         break;
      }
   }
}