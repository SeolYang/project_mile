#pragma once

#include "Component.h"

namespace Mile
{
   class Material;
   class Mesh;
   class MEAPI MeshRenderComponent : public Component
   {
      RegisterComponent( MeshRenderComponent )
   public:
      MeshRenderComponent( Entity* entity ) : 
         m_mesh( nullptr ),
         Component( entity )
      {
      }

      virtual std::string Serialize( ) const override;
      virtual void DeSerialize( const json& jsonData ) override;

      void SetMesh( Mesh* mesh ) { m_mesh = mesh; }
      Mesh* GetMesh( ) const { return m_mesh; }

      void SetMaterial( std::weak_ptr<Material> material ) { m_material = material; }
      std::weak_ptr<Material> GetMaterial( ) const { return m_material; }

   private:
      Mesh*                   m_mesh;
      std::weak_ptr<Material> m_material;

   };
}
