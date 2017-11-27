#pragma once

#include "Resource.h"

namespace Mile
{
   class Entity;
   class Mesh;
   class Material;
   class World;
   class MEAPI Model : public Resource
   {
   public:
      Model( Context* context, const String& filePath );
      virtual ~Model( );

      virtual bool Init( ) override;

      void AddMesh( Mesh* mesh );
      Mesh* GetMeshByName( const std::string& name );

      static Entity* Instantiate( Model* target, World* targetWorld );

   private:
      Entity*                 m_instance;
      std::vector<Mesh*>      m_meshes;

      std::string             m_serializedInstance;

   };
}