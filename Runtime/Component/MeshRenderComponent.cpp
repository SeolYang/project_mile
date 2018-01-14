#include "MeshRenderComponent.h"
#include "Core\Entity.h"
#include "Core\Context.h"
#include "Resource\ResourceManager.h"
#include "Resource\Model.h"
#include "Resource\Material.h"
#include "Rendering\Mesh.h"

namespace Mile
{
   std::string MeshRenderComponent::Serialize( ) const
   {
      //auto model = m_mesh->Ge
      // { "Mesh": { "Model": (Model filepath), "Name": (Mesh Name) },
      // "Material": (Material filepath) }
      //std::string res = "{ \"Mesh\": " + 
      std::string res = "{ " + Component::Serialize( ) +
         ", \"Mesh\": { \"Model\": \"" + WString2String( m_mesh->GetModelPath( ) ) + 
         "\", \"Name\": \"" + m_mesh->GetName( ) + "\" }, "
         " \"Material\": \"" + WString2String( m_material->GetPath( ) ) + "\" }";
      return res;
   }

   void MeshRenderComponent::DeSerialize( const json& jsonData )
   {
      Component::DeSerialize( jsonData );
      auto resMng = m_entity->GetContext( )->GetSubSystem<ResourceManager>( );
      json meshData = jsonData[ "Mesh" ];
      auto loadedModel = resMng->Load<Model>( String2WString( meshData[ "Model" ] ) );
      if ( loadedModel != nullptr )
      {
         m_mesh = loadedModel->GetMeshByName( meshData[ "Name" ] );

         m_material = resMng->Load<Material>( String2WString( jsonData[ "Material" ] ) );
         // Begin
         //json meshData = jsonData[ "Mesh" ];
         //meshData["Model"] -> Resource Load
         // m_mesh = LoadedModel->GetMeshByName( meshData["Name"] )
         //jsonData[ "Material" ] -> Resource Load
         // m_material = LoadedMaterial
         // End
      }
   }
}