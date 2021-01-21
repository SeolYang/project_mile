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
   RegisterComponent(MeshRenderComponent);

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
      json meshData = jsonData["Mesh"];
      Model* loadedModel = nullptr;

      switch (static_cast<EStaticMeshType>(meshData["Type"]))
      {
      case EStaticMeshType::External:
         loadedModel = resMng->Load<Model>(String2WString(meshData["Model"]));
         if (loadedModel != nullptr)
         {
            m_mesh = loadedModel->GetMeshByName(String2WString(meshData["Name"]));
         }
         break;

      case EStaticMeshType::Cube:
         m_mesh = Engine::GetRenderer()->GetCubeMesh();
         break;

      case EStaticMeshType::Quad:
         m_mesh = Engine::GetRenderer()->GetQuadMesh();
         break;
      }

      m_material = resMng->Load<Material>(String2WString(jsonData["Material"]));
   }
}