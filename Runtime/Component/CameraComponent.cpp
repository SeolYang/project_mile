#include "Component/CameraComponent.h"

namespace Mile
{
   RegisterComponent(CameraComponent);

   CameraComponent::CameraComponent(Entity* entity) :
      m_fov(45.0f),
      m_nearPlane(1.0f),
      m_farPlane(500.0f),
      m_clearColor(Vector4(0.133f, 0.137f, 0.15f, 1.0f)),
      Component(entity)
   {
   }

   json CameraComponent::Serialize() const
   {
      json serialized = Component::Serialize();
      serialized["FOV"] = m_fov;
      serialized["NearPlane"] = m_nearPlane;
      serialized["FarPlane"] = m_farPlane;
      serialized["ClearColor"] = m_clearColor.Serialize();
      return serialized;
   }

   void CameraComponent::DeSerialize(const json& jsonData)
   {
      Component::DeSerialize(jsonData);
      m_fov = GetValueSafelyFromJson(jsonData, "FOV", 45.0f);
      m_nearPlane = GetValueSafelyFromJson(jsonData, "NearPlane", 1.0f);
      m_farPlane = GetValueSafelyFromJson(jsonData, "FarPlane", 500.0f);
      m_clearColor.DeSerialize(jsonData["ClearColor"]);
   }

}