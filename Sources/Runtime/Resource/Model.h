#pragma once
#include "Resource/Resource.h"
#include "Core/Logger.h"

namespace Mile
{
   DECLARE_LOG_CATEGORY_EXTERN(MileModel, Log);

   class Entity;
   class Mesh;
   class Material;
   class World;
   class MEAPI Model : public Resource
   {
   public:
      Model(Context* context, const String& filePath);
      virtual ~Model();

      virtual bool Init() override;

      void AddMesh(Mesh* mesh);
      Mesh* GetMeshByName(const std::wstring& name);

      static Entity* Instantiate( Model* target, World* targetWorld, const String& entityName = TEXT("Entity"));

   private:
      Entity* m_instance;
      std::vector<Mesh*>      m_meshes;
      std::string             m_serializedInstance;

   };
}