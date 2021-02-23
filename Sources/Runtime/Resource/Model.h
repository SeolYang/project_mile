#pragma once
#include "Resource/Resource.h"
#include "Core/Logger.h"

namespace Mile
{
   DECLARE_LOG_CATEGORY_EXTERN(MileModel, Log);

   struct MEAPI ModelLoadParams
   {
      json Serialize() const
      {
         json object;
         object["ConvertToLeftHanded"] = ConvertToLeftHanded;
         object["Triangulate"] = Triangulate;
         object["CalcTangentSpace"] = CalcTangentSpace;
         object["GenUVs"] = GenUVs;
         object["GenSmoothNormals"] = GenSmoothNormals;
         object["PreTransformVertices"] = PreTransformVertices;
         return object;
      }

      void DeSerialize(const json& data)
      {
         ConvertToLeftHanded = GetValueSafelyFromJson(data, "ConvertToLeftHanded", true);
         Triangulate = GetValueSafelyFromJson(data, "Triangulate", true);
         CalcTangentSpace = GetValueSafelyFromJson(data, "CalcTangentSpace", true);
         GenUVs = GetValueSafelyFromJson(data, "GenUVs", true);
         GenSmoothNormals = GetValueSafelyFromJson(data, "GenSmoothNormals", true);
         PreTransformVertices = GetValueSafelyFromJson(data, "PreTransformVertices", true);
      }

      bool ConvertToLeftHanded = true;
      bool Triangulate = true;
      bool CalcTangentSpace = true;
      bool GenUVs = true;
      bool GenSmoothNormals = true;
      bool PreTransformVertices = true;
   };

   class Entity;
   class Mesh;
   class Material;
   class World;
   class MEAPI Model : public Resource
   {
   public:
      Model(ResourceManager* resMng);
      virtual ~Model();

      virtual bool Init(const String& filePath) override;

      void AddMesh(Mesh* mesh);
      Mesh* GetMeshByName(const std::wstring& name);

      ModelLoadParams GetLoadParameters() const { return m_loadParams; }
      ModelLoadParams& ModelLoadParameters() { return m_loadParams; }

      void LoadMetafile() override;
      void SaveMetafile() override;

      static Entity* Instantiate( Model* target, World* targetWorld, const String& entityName = TEXT("Entity"));

   private:
      Entity* m_instance;
      std::vector<Mesh*>      m_meshes;
      std::string             m_serializedInstance;
      ModelLoadParams m_loadParams;

   };
}