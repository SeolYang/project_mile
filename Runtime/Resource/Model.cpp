#include "Model.h"
#include "ModelLoader.h"
#include "Core\Entity.h"
#include "Rendering\Mesh.h"
#include "Core\World.h"
#include <assimp\scene.h>
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>

namespace Mile
{
   Model::Model( Context* context, const String& filePath ) :
      m_instance( nullptr ),
      Resource( context, filePath, ResourceType::RT_Model )
   {
   }

   Model::~Model( )
   {
      for ( auto mesh : m_meshes )
      {
         SafeDelete( mesh );
      }
   }

   bool Model::Init( )
   {
      if ( m_context == nullptr || m_instance != nullptr )
      {
         return false;
      }

      m_instance = ModelLoader::LoadModel( m_context, this, m_path );
      m_serializedInstance = m_instance->Serialize( );

      return true;
   }

   void Model::AddMesh( Mesh* mesh )
   {
      m_meshes.push_back( mesh );
   }

   Mesh* Model::GetMeshByName( const std::string& name )
   {
      for ( auto mesh : m_meshes )
      {
         if ( mesh->GetName( ) == name )
         {
            return mesh;
         }
      }

      return nullptr;
   }

   Entity* Model::Instantiate( Model* target, World* targetWorld )
   {
      Entity* tempEntity = targetWorld->CreateEntity( TEXT( "" ) );
      tempEntity->DeSerialize( json::parse( target->m_serializedInstance ) );
      return tempEntity;
   }
}