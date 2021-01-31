#pragma once
#include "Component/ComponentRegister.h"

namespace Mile
{
   class Material;
   class Mesh;
   class MEAPI MeshRenderComponent : public Component
   {
      ComponentBegin(MeshRenderComponent)

   public:
      MeshRenderComponent(Entity* entity) :
         m_mesh(nullptr),
         m_material(nullptr),
         Component(entity)
      {
         m_bCanEverUpdate = false;
      }

      //virtual std::string Serialize( ) const override;
      virtual json Serialize() const override;
      virtual void DeSerialize(const json& jsonData) override;

      void SetMesh(Mesh* mesh) { m_mesh = mesh; }
      Mesh* GetMesh() const { return m_mesh; }

      void SetMaterial(Material* material) { m_material = material; }
      Material* GetMaterial() const { return m_material; }

   private:
      Mesh* m_mesh;
      Material* m_material;

   };
}
