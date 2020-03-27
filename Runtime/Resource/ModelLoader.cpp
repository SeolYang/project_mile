#include "Resource/ModelLoader.h"
#include "Resource/ResourceManager.h"
#include "Resource/Model.h"
#include "Resource/Material.h"
#include "Resource/Texture2D.h"
#include "Core/Context.h"
#include "Core/Logger.h"
#include "Component/MeshRenderComponent.h"
#include "GameFramework/Entity.h"
#include "Rendering/Mesh.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Quaternion.h"
#include "Math/Vertex.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/pbrmaterial.h>

namespace Mile
{
   Context* ModelLoader::contextInst = nullptr;
   RendererDX11* ModelLoader::rendererInst = nullptr;

   Entity* ModelLoader::LoadModel(Context* context, Model* target, const String& filePath)
   {
      if (context == nullptr)
      {
         return false;
      }

      if (context != contextInst)
      {
         contextInst = context;
         rendererInst = nullptr;
      }

      if (rendererInst == nullptr)
      {
         rendererInst = context->GetSubSystem<RendererDX11>();
      }

      Assimp::Importer importer;
      auto scene = importer.ReadFile(WString2String(filePath),
         aiProcess_CalcTangentSpace |
         aiProcess_Triangulate |
         aiProcess_GenSmoothNormals |
         //aiProcess_SplitLargeMeshes |
         aiProcess_ConvertToLeftHanded |
         aiProcess_SortByPType |
         //aiProcess_GenUVCoords |
         aiProcess_MakeLeftHanded |
         aiProcess_PreTransformVertices);

      Entity* res = new Entity(nullptr, TEXT("model"));
      ReconstructEntityWithAiNode(scene, scene->mRootNode, target, res);

      importer.FreeScene();
      return res;
   }

   void ModelLoader::ReconstructEntityWithAiNode(const aiScene * scene, aiNode * node, Model * target, Entity * entity)
   {
      bool bIsNotValidCall = scene == nullptr || node == nullptr || entity == nullptr;
      if (bIsNotValidCall)
      {
         return;
      }

      // Create Mesh Entity first
      for (size_t idx = 0; idx < node->mNumMeshes; ++idx)
      {
         Entity* renderableMesh = new Entity(nullptr, TEXT(""));
         ReconstructMeshWithAiMesh(scene, scene->mMeshes[idx], target, renderableMesh);
         entity->AttachChild(renderableMesh);
      }

      // Create children Entity
      for (size_t idx = 0; idx < node->mNumChildren; ++idx)
      {
         if (node->mNumMeshes == 0)
         {
            ReconstructEntityWithAiNode(scene, node->mChildren[idx], target, entity);
         }
         else
         {
            Entity* child = new Entity(nullptr, TEXT(""));
            ReconstructEntityWithAiNode(scene, node->mChildren[idx], target, child);
            entity->AttachChild(child);
         }
      }
   }

   void ModelLoader::ReconstructMeshWithAiMesh(const aiScene * scene, aiMesh * mesh, Model * target, Entity * entity)
   {
      bool bValidCall = scene != nullptr && mesh != nullptr && entity != nullptr;
      if (!bValidCall)
      {
         return;
      }

      /* Mesh Setup */
      std::vector<VertexPosTexNTB> verticies(mesh->mNumVertices);
      std::vector<unsigned int> indices((size_t)mesh->mNumFaces * 3);

      bool bHasUV = mesh->HasTextureCoords(0);
      bool bHasNormals = mesh->HasNormals();
      bool bHasTangentAndBiNormal = mesh->HasTangentsAndBitangents();

      auto aiVerticies = mesh->mVertices;

      for (unsigned int idx = 0; idx < mesh->mNumVertices; ++idx)
      {
         auto& vertex = verticies[idx];

         auto pos = mesh->mVertices[idx];
         vertex.Position = Vector4(pos.x, pos.y, pos.z, 1.0f);

         if (bHasUV)
         {
            auto uvw = mesh->mTextureCoords[0][idx];
            vertex.TexCoord = Vector2(uvw.x, uvw.y);
         }

         if (bHasNormals)
         {
            auto normal = mesh->mNormals[idx];
            vertex.Normal = Vector3(normal.x, normal.y, normal.z);
         }

         if (bHasTangentAndBiNormal)
         {
            auto tan = mesh->mTangents[idx];
            vertex.Tangent = Vector4(tan.x, tan.y, tan.z, 0.0f);

            auto biNormal = mesh->mBitangents[idx];
            vertex.BiTangent = Vector4(biNormal.x, biNormal.y, biNormal.z, 0.0f);
         }
      }

      for (size_t idx = 0; idx < mesh->mNumFaces; ++idx)
      {
         auto face = mesh->mFaces[idx];
         indices[idx * 3 + 0] = face.mIndices[0];
         indices[idx * 3 + 1] = face.mIndices[1];
         indices[idx * 3 + 2] = face.mIndices[2];
      }

      auto meshName = String2WString(mesh->mName.C_Str());
      entity->SetName(meshName);
      Mesh* newMesh = new Mesh(rendererInst,
         meshName,
         target->GetPath());
      newMesh->Init(verticies, indices);
      target->AddMesh(newMesh);

      auto renderComponent = entity->AddComponent<MeshRenderComponent>();
      renderComponent->SetMesh(newMesh);

      /* Material Setup  */
      auto resMng = contextInst->GetSubSystem<ResourceManager>();
      if (resMng == nullptr)
      {
         MELog(contextInst,
            TEXT("ModelLoader::ReconstructMeshWithAiMesh"),
            ELogType::FATAL, TEXT("Couldn't load ResourceManager."),
            true);
      }

      auto matPath = target->GetFolder()
         + target->GetName()
         + TEXT("_")
         + meshName
         + TEXT(".material");

      Material* foundedMat = resMng->Load<Material>(matPath);
      if (foundedMat == nullptr)
      {
         foundedMat = resMng->Create<Material>(target->GetFolder()
            + target->GetName()
            + TEXT("_")
            + meshName
            + TEXT(".material"));

         if (mesh->mMaterialIndex >= 0)
         {
            // BaseColor, Normal, Metallic, Roughness, AO, Emissive
            aiString baseColor;
            aiString metallicRoughness;
            aiString emissive;
            aiString normal;
            aiString ao;

            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            material->GetTexture(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_TEXTURE, &baseColor);
            material->GetTexture(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLICROUGHNESS_TEXTURE, &metallicRoughness);
            material->GetTexture(aiTextureType::aiTextureType_EMISSIVE, 0, &emissive);
            material->GetTexture(aiTextureType::aiTextureType_AMBIENT_OCCLUSION, 0, &ao);
            material->GetTexture(aiTextureType::aiTextureType_NORMALS, 0, &normal);

            foundedMat->SetTexture2D(
               MaterialTextureProperty::BaseColor,
               resMng->Load<Texture2D>(target->GetFolder() + String2WString(baseColor.C_Str())));
            foundedMat->SetTexture2D(
               MaterialTextureProperty::Emissive,
               resMng->Load<Texture2D>(target->GetFolder() + String2WString(emissive.C_Str())));
            foundedMat->SetTexture2D(
               MaterialTextureProperty::MetallicRoughness,
               resMng->Load<Texture2D>(target->GetFolder() + String2WString(metallicRoughness.C_Str())));
            foundedMat->SetTexture2D(
               MaterialTextureProperty::Normal,
               resMng->Load<Texture2D>(target->GetFolder() + String2WString(normal.C_Str())));
            foundedMat->SetTexture2D(
               MaterialTextureProperty::AO,
               resMng->Load<Texture2D>(target->GetFolder() + String2WString(ao.C_Str())));
         }

         foundedMat->Save();
         foundedMat->Init();
      }

      renderComponent->SetMaterial(foundedMat);
   }
}