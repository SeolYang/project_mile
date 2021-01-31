#include "Component/MeshRenderComponent.h"
#include "Core/Context.h"
#include "Core/Engine.h"
#include "Rendering/RendererDX11.h"
#include "GameFramework/Entity.h"
#include "Resource/ResourceManager.h"
#include "Resource/Model.h"
#include "Resource/Material.h"
#include "Rendering/Mesh.h"

namespace Mile
{
   DefineComponent(MeshRenderComponent);

   json MeshRenderComponent::Serialize() const
   {
      json serialized = Component::Serialize();

      json meshData;
      meshData["Model"] = WString2String(m_mesh->GetModelPath());
      meshData["Name"] = WString2String(m_mesh->GetName());
      meshData["Type"] = static_cast<int>(m_mesh->GetMeshType());

      serialized["Mesh"] = meshData;
      serialized["Material"] = WString2String(m_material->GetPath());

      return serialized;
   }

   void MeshRenderComponent::DeSerialize(const json& jsonData)
   {
      Component::DeSerialize(jsonData);
      auto resMng = m_entity->GetContext()->GetSubSystem<ResourceManager>();
      json meshData = GetValueSafelyFromJson(jsonData, "Mesh", json());
      Model* loadedModel = nullptr;

      switch (static_cast<EStaticMeshType>(GetValueSafelyFromJson(meshData, "Type", 0)))
      {
      case EStaticMeshType::External:
         loadedModel = resMng->Load<Model>(String2WString(GetValueSafelyFromJson(meshData, "Model", std::string())));
         if (loadedModel != nullptr)
         {
            m_mesh = loadedModel->GetMeshByName(String2WString(GetValueSafelyFromJson(meshData, "Name", std::string())));
         }
         break;

      case EStaticMeshType::Cube:
         m_mesh = Engine::GetRenderer()->GetCubeMesh();
         break;

      case EStaticMeshType::Quad:
         m_mesh = Engine::GetRenderer()->GetQuadMesh();
         break;
      }

      m_material = resMng->Load<Material>(String2WString(GetValueSafelyFromJson(jsonData, "Material", std::string())));
   }
}