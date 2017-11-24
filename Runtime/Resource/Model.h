#pragma once

#include "Resource.h"

namespace Mile
{
   class Entity;
   class Mesh;
   class Material;
   class MEAPI Model : public Resource
   {
   public:
      Model( Context* context, const String& filePath ) :
         Resource( context, filePath, ResourceType::RT_Model )
      {
      }

      virtual ~Model( ) 
      { 
         for ( auto mesh : m_meshes )
         {
            SafeDelete( mesh );
         }
      }

      virtual bool Init( ) override;

      void AddMesh( Mesh* mesh );
      Mesh* GetMeshByName( const std::string& name );

   private:
      Entity*                 m_instance;
      std::vector<Mesh*>      m_meshes;

   };
}