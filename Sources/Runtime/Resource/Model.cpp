#include "Resource/Model.h"
#include "Resource/ModelLoader.h"
#include "Rendering/Mesh.h"
#include "GameFramework/Entity.h"
#include "GameFramework/World.h"

namespace Mile
{
   DEFINE_LOG_CATEGORY(MileModel);

   Model::Model(Context* context, const String& filePath) :
      m_instance(nullptr),
      Resource(context, filePath, ResourceType::Model)
   {
   }

   Model::~Model()
   {
      SafeDelete(m_instance);
      for (auto mesh : m_meshes)
      {
         SafeDelete(mesh);
      }
   }

   bool Model::Init()
   {
      if (m_context == nullptr || m_instance != nullptr)
      {
         return false;
      }

      m_instance = ModelLoader::LoadModel(m_context, this, m_path);
      if (m_instance == nullptr)
      {
         ME_LOG(MileModel, Warning, TEXT("Failed to load model from ") + m_path);
         return false;
      }

      m_serializedInstance = m_instance->Serialize().dump();
      return true;
   }

   void Model::AddMesh(Mesh* mesh)
   {
      m_meshes.push_back(mesh);
   }

   Mesh* Model::GetMeshByName(const std::wstring& name)
   {
      for (auto mesh : m_meshes)
      {
         if (mesh->GetName() == name)
         {
            return mesh;
         }
      }

      return nullptr;
   }

   Entity* Model::Instantiate(Model* target, World* targetWorld, const String& entityName)
   {
      Entity* tempEntity = targetWorld->CreateEntity(TEXT(""));
      auto serialized = json::parse(target->m_serializedInstance);
      tempEntity->DeSerialize(serialized);
      tempEntity->SetName(entityName);
      return tempEntity;
   }
}