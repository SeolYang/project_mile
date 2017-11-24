#pragma once

#include "Core\Helper.h"

class aiNode;
class aiMesh;
class aiScene;
namespace Mile
{
   class Model;
   class Context;
   class RendererDX11;
   class Entity;
   class Mesh;
   class MEAPI ModelLoader
   {
   public:
      Entity* LoadModel( Context* context, Model* target, const String& filePath );

   private:
      static void ReconstructEntityWithAiNode( const aiScene* scene, aiNode* node, Model* target, Entity* entity );
      static void ReconstructMeshWithAiMesh( const aiScene* scene, aiMesh* mesh, Model* target, Entity* entity );

   private:
      static Context*       contextInst;
      static RendererDX11*  rendererInst;

   };
}
