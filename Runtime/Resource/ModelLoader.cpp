#include "ModelLoader.h"
#include <assimp\scene.h>
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include "Component\MeshRenderComponent.h"
#include "Core\Context.h"
#include "Core\Entity.h"
#include "Rendering\Mesh.h"
#include "Resource\Model.h"
#include "Resource\Material.h"
#include "Math\Vector3.h"
#include "Math\Vector4.h"
#include "Math\Quaternion.h"
#include "Math\Vertex.h"
#include "ResourceManager.h"

namespace Mile
{
   Context* ModelLoader::contextInst = nullptr;
   RendererDX11* ModelLoader::rendererInst = nullptr;

   Entity* ModelLoader::LoadModel( Context* context, Model* target, const String& filePath )
   {
      if ( context == nullptr )
      {
         return false;
      }

      if ( context != contextInst )
      {
         contextInst = context;
         rendererInst = nullptr;
      }

      if ( rendererInst == nullptr )
      {
         rendererInst = context->GetSubSystem<RendererDX11>( );
      }

      Assimp::Importer importer;
      auto scene = importer.ReadFile( WString2String( filePath ),
                                      aiProcess_CalcTangentSpace |
                                      aiProcess_Triangulate |
                                      aiProcess_GenSmoothNormals |
                                      aiProcess_SplitLargeMeshes |
                                      aiProcess_ConvertToLeftHanded |
                                      aiProcess_SortByPType );

      Entity* res = new Entity( nullptr, TEXT( "" ) );
      ReconstructEntityWithAiNode( scene, scene->mRootNode, target, res );

      importer.FreeScene( );
      return res;
   }

   void ModelLoader::ReconstructEntityWithAiNode( const aiScene* scene, aiNode* node, Model* target, Entity* entity )
   {
      bool bIsNotValidCall = scene == nullptr || node == nullptr || entity == nullptr;
      if ( bIsNotValidCall )
      {
         return;
      }

      // Create Mesh Entity first
      for ( size_t idx = 0; idx < node->mNumMeshes; ++idx )
      {
         Entity* renderableMesh = new Entity( nullptr, TEXT( "" ) );
         ReconstructMeshWithAiMesh( scene, scene->mMeshes[idx], target, renderableMesh );
         entity->AttachChild( renderableMesh );
      }

      // Create children Entity
      for ( size_t idx = 0; idx < node->mNumChildren; ++idx )
      {
         Entity* child = new Entity( nullptr, TEXT( "" ) );
         ReconstructEntityWithAiNode( scene, node->mChildren[ idx ], target, child );
         entity->AttachChild( child );
      }
   }

   void ModelLoader::ReconstructMeshWithAiMesh( const aiScene* scene, aiMesh* mesh, Model* target, Entity* entity )
   {
      bool bValidCall = scene != nullptr && mesh != nullptr && entity != nullptr;
      if ( !bValidCall )
      {
         return;
      }

      // Reserve space
      std::vector<VertexPosTexNT> verticies( mesh->mNumVertices );
      std::vector<unsigned int> indices( mesh->mNumFaces * 3 );

      bool bHasUV = mesh->HasTextureCoords( 0 );
      bool bHasNormals = mesh->HasNormals( );
      bool bHasTangentAndBiNormal = mesh->HasTangentsAndBitangents( );

      auto aiVerticies = mesh->mVertices;

      for ( unsigned int idx = 0; idx < mesh->mNumVertices; ++idx )
      {
         auto& vertex = verticies[ idx ];

         auto pos = mesh->mVertices[ idx ];
         vertex.Position = Vector4( pos.x, pos.y, pos.z, 1.0f );

         if ( bHasUV )
         {
            auto uvw = mesh->mTextureCoords[ 0 ][ idx ];
            vertex.TexCoord = Vector2( uvw.x, uvw.y );
         }

         if ( bHasNormals )
         {
            auto normal = mesh->mNormals[ idx ];
            vertex.Normal = Vector3( normal.x, normal.y, normal.z );
         }

         if ( bHasTangentAndBiNormal )
         {
            auto tan = mesh->mTangents[ idx ];
            vertex.Tangent = Vector4( tan.x, tan.y, tan.z, 0.0f );
         }
      }

      for ( unsigned int idx = 0; idx < mesh->mNumFaces; ++idx )
      {
         auto face = mesh->mFaces[ idx ];
         indices[ idx * 3 + 0 ] = face.mIndices[ 0 ];
         indices[ idx * 3 + 1 ] = face.mIndices[ 1 ];
         indices[ idx * 3 + 2 ] = face.mIndices[ 2 ];
      }

      // Create Material (or just load)
      Mesh* newMesh = new Mesh( rendererInst,
                                mesh->mName.C_Str( ),
                                target->GetPath( ) );
      newMesh->Init( verticies, indices );
      target->AddMesh( newMesh );

      // Todo Implement mesh render component
      auto renderComponent = entity->AddComponent<MeshRenderComponent>( );
      renderComponent->SetMesh( newMesh );
      // Set Material
      // Create model dir + material name
      auto resMng = contextInst->GetSubSystem<ResourceManager>( );
      auto newMat = resMng->Create<Material>( target->GetFolder() + target->GetName( ) + std::to_wstring( mesh->mMaterialIndex )
                                 + TEXT(".material"));
      newMat->Save( );

      // @TODO: Get Diffuse/Specular Texture names

      renderComponent->SetMaterial( newMat );
   }
}