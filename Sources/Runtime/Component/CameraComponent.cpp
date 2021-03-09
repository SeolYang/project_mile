#include "Component/CameraComponent.h"
#include "Core/Engine.h"
#include "Core/ImGuiHelper.h"
#include "Resource/ResourceManager.h"
#include "Resource/RenderTexture.h"
#include "Rendering/Light.h"

namespace Mile
{
   DefineComponent(CameraComponent);

   CameraComponent::CameraComponent(Entity* entity) :
      m_fov(45.0f),
      m_nearPlane(1.0f),
      m_farPlane(500.0f),
      m_aperture(1.0f),
      m_shutterSpeed(0.001f),
      m_sensitivity(100.0f),
      m_clearColor(Vector4(0.133f, 0.137f, 0.15f, 1.0f)),
      m_renderTexture(nullptr),
      m_meteringMode(EMeteringMode::Manual),
      m_expComp(0.0f),
      m_lightAdaptionSpeed(3.0f),
      m_darkAdaptionSpeed(1.0f),
      m_minBrightness(0.03f),
      m_maxBrightness(2.f),
      Component(entity)
   {
      m_bCanEverUpdate = false;
   }

   json CameraComponent::Serialize() const
   {
      json serialized = Component::Serialize();
      serialized["FOV"] = m_fov;
      serialized["NearPlane"] = m_nearPlane;
      serialized["FarPlane"] = m_farPlane;
      serialized["ClearColor"] = m_clearColor.Serialize();
      serialized["MeteringMode"] = static_cast<unsigned int>(m_meteringMode);
      serialized["Aperture"] = m_aperture;
      serialized["ShutterSpeed"] = m_shutterSpeed;
      serialized["Sensitivity"] = m_sensitivity;
      serialized["LightAdaptionSpeed"] = m_lightAdaptionSpeed;
      serialized["DarkAdaptionSpeed"] = m_darkAdaptionSpeed;
      serialized["MinBrightness"] = m_minBrightness;
      serialized["MaxBrightness"] = m_maxBrightness;
      serialized["ExpComp"] = m_expComp;
      if (m_renderTexture == nullptr)
      {
         serialized["RenderTexture"] = NULL_TEXT_STD;
      }
      else
      {
         serialized["RenderTexture"] = WString2String(m_renderTexture->GetPath());
      }

      return serialized;
   }

   void CameraComponent::DeSerialize(const json& jsonData)
   {
      Component::DeSerialize(jsonData);
      m_fov = GetValueSafelyFromJson(jsonData, "FOV", 45.0f);
      m_nearPlane = GetValueSafelyFromJson(jsonData, "NearPlane", 1.0f);
      m_farPlane = GetValueSafelyFromJson(jsonData, "FarPlane", 500.0f);
      m_clearColor.DeSerialize(jsonData["ClearColor"]);

      m_meteringMode = static_cast<EMeteringMode>(GetValueSafelyFromJson(jsonData, "MeteringMode", (unsigned int)0));

      m_aperture = GetValueSafelyFromJson(jsonData, "Aperture", 1.0f);
      m_shutterSpeed = GetValueSafelyFromJson(jsonData, "ShutterSpeed", 0.001f);
      m_sensitivity = GetValueSafelyFromJson(jsonData, "Sensitivity", 100.0f);

      m_lightAdaptionSpeed = GetValueSafelyFromJson(jsonData, "LightAdaptionSpeed", 3.0f);
      m_darkAdaptionSpeed = GetValueSafelyFromJson(jsonData, "DarkAdaptionSpeed", 1.0f);
      m_minBrightness = GetValueSafelyFromJson(jsonData, "MinBrightness", 0.03f);
      m_maxBrightness = GetValueSafelyFromJson(jsonData, "MaxBrightness", 2.0f);

      m_expComp = GetValueSafelyFromJson(jsonData, "ExpComp", 0.0f);

      std::string renderTexture = GetValueSafelyFromJson(jsonData, "RenderTexture", std::string());
      if (renderTexture == NULL_TEXT_STD)
      {
         m_renderTexture = nullptr;
      }
      else
      {
         auto resMng = Engine::GetResourceManager();
         if (resMng != nullptr)
         {
            m_renderTexture = resMng->Load<RenderTexture>(String2WString(renderTexture));
         }
      }
   }

   float CameraComponent::Exposure() const
   {
      float exposure = EV100(m_aperture, m_shutterSpeed, m_sensitivity) - m_expComp;
      if (m_meteringMode == EMeteringMode::AutoExposureBasic)
      {
         return 1.0f;
      }
      return ExposureNormalizationFactor(exposure);
   }

   void CameraComponent::OnGUI()
   {
      /** @TODO Add Render Texture */
      GUI::FloatInput("Field Of View", m_fov, 1.0f, 10.0f, 90.0f, true);
      GUI::FloatInput("Near Plane", m_nearPlane, 1.0f, 0.0f, FLT_MAX);
      GUI::FloatInput("Far Plane", m_farPlane, 1.0f, 0.0f, FLT_MAX);
      GUI::Vector4Input("Clear Color", m_clearColor, 0.1f, 0.0f, 1.0f);

      if (GUI::TreeNode("Exposure", true))
      {
         std::vector<std::string> meteringModeItems = { "Manual", "Auto Exposure Basic", "Auto Exposure Histogram" };
         unsigned int meteringModeIdx = static_cast<unsigned int>(m_meteringMode);
         if (GUI::Combobox("Metering Mode", meteringModeItems, meteringModeItems[meteringModeIdx], meteringModeIdx))
         {
            m_meteringMode = static_cast<EMeteringMode>(meteringModeIdx);
         }

         switch (m_meteringMode)
         {
         case EMeteringMode::AutoExposureBasic:
            GUI::FloatInput("Light Adaption Speed", m_lightAdaptionSpeed, 0.1f, 0.0001f);
            GUI::FloatInput("Dark Adaption Speed", m_darkAdaptionSpeed, 0.1f, 0.0001f);
            GUI::FloatInput("Min Brightness", m_minBrightness, 0.1f, 0.0f);
            GUI::FloatInput("Max Brightness", m_maxBrightness, 0.1f, 0.0f);
            break;
         case EMeteringMode::Manual:
            GUI::FloatInput("Aperture", m_aperture, 0.1f, 1.0f, 16.0f, false, "f/%.03f");
            GUI::FloatInput("Shutter Speed", m_shutterSpeed, 0.1f, 0.0f, FLT_MAX, false, "%0.5f seconds");
            GUI::FloatInput("Sensitivity", m_sensitivity, 0.1f, 0.0f, FLT_MAX, false, "%0.3f ISO");
            break;
         }

         GUI::FloatInput("Exposure Compensation", m_expComp, 0.1f, -20.0f, 20.0f);
         GUI::TreePop();
      }
   }
}