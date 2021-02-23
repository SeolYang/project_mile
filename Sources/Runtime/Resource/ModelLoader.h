#pragma once
#include "Core/Logger.h"

struct aiNode;
struct aiMesh;
struct aiScene;
namespace Mile
{
   DECLARE_LOG_CATEGORY_EXTERN(MileModelLoader, Log);

   class Model;
   class Context;
   class World;
   class RendererDX11;
   class ResourceManager;
   class Entity;
   class Mesh;
   class MEAPI ModelLoader
   {
   public:
      ModelLoader(ResourceManager* resMng);
      Entity* LoadModel(Model* target, const String& filePath);

   private:
      void ReconstructEntityWithAiNode(const aiScene* scene, aiNode* node, Model* target, Entity* entity);
      void ReconstructMeshWithAiMesh(const aiScene* scene, aiMesh* mesh, Model* target, Entity* entity);

   private:
      ResourceManager* m_resMng;
      RendererDX11* m_renderer;

   };
}
