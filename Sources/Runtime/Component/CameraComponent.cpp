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
      GUI::FloatInput("Field Of View", m_fov, 1.0f, 10.0f, 90.0f, true);
      GUI::FloatInput("Near", m_nearPlane, 0.1f, 0.01f, FLT_MAX);
      GUI::FloatInput("Far", m_farPlane, 0.1f, 0.01f, FLT_MAX);
      GUI::Vector4Input("Clear Color", m_clearColor, 0.1f, 0.0f, 1.0f);
      /** @todo Render Texture */
   }
}