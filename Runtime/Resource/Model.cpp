#include "Model.h"
#include "Rendering\Mesh.h"
#include <assimp\scene.h>
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>

namespace Mile
{
   bool Model::Init( )
   {
      if ( m_context == nullptr || m_instance != nullptr )
      {
         return false;
      }

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
}