#pragma once
#include "Resource/Resource.h"
#include "Math/Vector2.h"
#include "Math/Vector4.h"

namespace Mile
{
   enum class MaterialTextureProperty
   {
      BaseColor,
      Emissive,
      MetallicRoughness,
      AO,
      Normal,
   };

   enum class MaterialFactorProperty
   {
      BaseColor,
      Emissive,
      Metallic,
      Roughness,
   };

   enum class EMaterialType : unsigned int
   {
      Opaque = 0,
      Translucent,
      Water
   };

   class Texture2D;
   class MEAPI Material : public Resource
   {
   public:
      Material(Context* context, const String& filePath);

      virtual bool Init() override;
      virtual bool SaveTo(const String& filePath) override;

      void SetTexture2D(MaterialTextureProperty prop, Texture2D* texture);
      Texture2D* GetTexture2D(MaterialTextureProperty propertyType) const;

      void SetScalarFactor(MaterialFactorProperty prop, float factor);
      void SetVector4Factor(MaterialFactorProperty prop, const Vector4& factor);
      float GetScalarFactor(MaterialFactorProperty prop) const;
      Vector4 GetVector4Factor(MaterialFactorProperty prop) const;

      void SetMaterialType(EMaterialType type) { m_materialType = type; }
      EMaterialType GetMaterialType() const { return m_materialType; }

      virtual json Serialize() const override;
      virtual void DeSerialize(const json& jsonData) override;

   private:
      EMaterialType m_materialType;
      Texture2D* m_baseColor;
      Texture2D* m_emissive;
      Texture2D* m_metallicRoughness;
      Texture2D* m_ao;
      Texture2D* m_normal;

      Vector4  m_baseColorFactor;
      Vector4  m_emissiveFactor;
      float		m_metallicFactor;
      float		m_roughnessFactor;

   };
}