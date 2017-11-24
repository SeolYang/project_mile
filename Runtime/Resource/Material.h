#pragma once

#include "Resource.h"

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
         m_specIntensity( 0.0f ),
         Resource( context, filePath, ResourceType::RT_Material )
      {
      }

      virtual bool Init( ) override;
      virtual bool SaveTo( const String& filePath ) override;

      // Specular Exponential
      // Specular Intensity
      // Diffuse map
      // Normal map

   private:
      float                      m_specExp;
      float                      m_specIntensity;
      std::weak_ptr<Texture2D>   m_diffuseMap;
      std::weak_ptr<Texture2D>   m_normalMap;

   };
}