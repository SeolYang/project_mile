#include "Component/CameraComponent.h"
#include "Core/Engine.h"
#include "Core/ImGuiHelper.h"
#include "Resource/ResourceManager.h"
#include "Resource/RenderTexture.h"

namespace Mile
{
   DefineComponent(CameraComponent);

   CameraComponent::CameraComponent(Entity* entity) :
      m_fov(45.0f),
      m_nearPlane(1.0f),
      m_farPlane(500.0f),
      m_aperture(1.0f),
      m_shutterSpeed(1.0f),
      m_sensitivity(100.0f),
      m_clearColor(Vector4(0.133f, 0.137f, 0.15f, 1.0f)),
      m_renderTexture(nullptr),
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
      serialized["Aperture"] = m_aperture;
      serialized["ShutterSpeed"] = m_shutterSpeed;
      serialized["Sensitivity"] = m_sensitivity;
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
      m_aperture = GetValueSafelyFromJson(jsonData, "Aperture", 1.0f);
      m_shutterSpeed = GetValueSafelyFromJson(jsonData, "ShutterSpeed", 1.0f);
      m_sensitivity = GetValueSafelyFromJson(jsonData, "Sensitivity", 100.0f);

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

   void CameraComponent::OnGUI()
   {
      /** @TODO Add Render Texture */
      GUI::FloatInput("Field Of View", m_fov, 1.0f, 10.0f, 90.0f, true);
      GUI::FloatInput("Near Plane", m_nearPlane, 1.0f, 0.0f, FLT_MAX);
      GUI::FloatInput("Far Plane", m_farPlane, 1.0f, 0.0f, FLT_MAX);
      GUI::Vector4Input("Clear Color", m_clearColor, 0.1f, 0.0f, 1.0f);
      GUI::FloatInput("Aperture(f-stops)", m_aperture, 1.4f, 4.0f);
      GUI::FloatInput("Shutter Speed(seconds)", m_shutterSpeed, 0.1f, 0.0f);
      GUI::FloatInput("Sensitivity(ISO)", m_sensitivity, 0.1f, 0.0f);
   }
}