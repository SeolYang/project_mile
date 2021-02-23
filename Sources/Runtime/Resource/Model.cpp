#include "Resource/Model.h"
#include "Resource/ResourceManager.h"
#include "Resource/ModelLoader.h"
#include "Rendering/Mesh.h"
#include "GameFramework/Entity.h"
#include "GameFramework/World.h"

namespace Mile
{
   DEFINE_LOG_CATEGORY(MileModel);

   Model::Model(ResourceManager* resMng) :
      m_instance(nullptr),
      Resource(resMng, ResourceType::Model)
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

   bool Model::Init(const String& filePath)
   {
      if (Resource::Init(filePath))
      {
         ModelLoader& loader = m_resMng->GetModelLoader();
         LoadMetafile();
         m_instance = loader.LoadModel(this, m_path);
         if (m_instance == nullptr)
         {
            ME_LOG(MileModel, Warning, TEXT("Failed to load model from ") + m_path);
            return false;
         }

         m_serializedInstance = m_instance->Serialize().dump();
         SucceedInit();
         return true;
      }

      return false;
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

   void Model::LoadMetafile()
   {
      auto metaPath = GetMetaPath();
      std::ifstream stream(metaPath);
      if (!stream.is_open())
      {
         ME_LOG(MileModel, Warning, TEXT("Failed to open stream from ") + metaPath);
      }

      std::string jsonStr;
      std::string temp;
      while (std::getline(stream, temp))
      {
         jsonStr += temp;
         jsonStr += '\n';
      }
      stream.close();

      if (!jsonStr.empty())
      {
         m_loadParams.DeSerialize(json::parse(jsonStr));
      }
   }

   void Model::SaveMetafile()
   {
      auto metaPath = GetMetaPath();
      std::ofstream stream(metaPath);
      if (!stream.is_open())
      {
         ME_LOG(MileModel, Warning, TEXT("Failed to open stream from ") + metaPath);
      }

      stream << m_loadParams.Serialize().dump(4);
      stream.close();
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