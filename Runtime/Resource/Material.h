#pragma once

#include "Resource.h"
#include "Math\Vector3.h"

namespace Mile
{
   enum class TextureType
   {
      DiffuseMap,
      NormalMap
   };

   class Texture2D;
   class MEAPI Material : public Resource
   {
   public:
      Material( Context* context, const String& filePath ) :
         m_specExp( 1.0f ),
         m_specAlbedo( Vector3( ) ),
         m_diffuseMap( nullptr ),
         m_normalMap( nullptr ),
         Resource( context, filePath, ResourceType::RT_Material )
      {
      }

      virtual bool Init( ) override;
      virtual bool SaveTo( const String& filePath ) override;

      // Specular Exponential
      float GetSpecularExp( ) const { return m_specExp; }
      void SetSpecularExp( float specExp ) { m_specExp = specExp; }
      // Specular Albedo
      Vector3 GetSpecularAlbedo( ) const { return m_specAlbedo; }
      void SetSpecularAlbedo( const Vector3& specAlbedo ) { m_specAlbedo = specAlbedo; }
      // Diffuse map/Texture
      Texture2D* GetDiffuseMap( ) const { return m_diffuseMap; }
      void SetDiffuseMap( Texture2D* diffuseMap ) { m_diffuseMap = diffuseMap; }
      // Normal map/Texture
      Texture2D* GetNormalMap( ) const { return m_normalMap; }
      void SetNormalMap( Texture2D* normalMap ) { m_normalMap = normalMap; }

   private:
      float                      m_specExp;
      Vector3                    m_specAlbedo;
      Texture2D*                 m_diffuseMap;
      Texture2D*                 m_normalMap;

   };
}