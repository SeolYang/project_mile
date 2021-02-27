#pragma once
#include "Resource/Resource.h"
#include "Core/Logger.h"
#include "Math/Vector2.h"
#include "Math/Vector4.h"
#include "Rendering/RenderingCore.h"

namespace Mile
{
   DECLARE_LOG_CATEGORY_EXTERN(MileMaterial, Log);

   DEFINE_CONSTANT_BUFFER(PackedMaterialParams)
   {
      Vector4 BaseColorFactor = Vector4::One();
      Vector4 MetallicRoughnessUV = Vector4::Zero();
      float SpecularFactor = 0.0f;
      float EmissiveFactor = 0.0f;
   };

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
      Specular,
      UVOffset,
   };

   enum class EMaterialType : unsigned int
   {
      Opaque = 0,
      Translucent,
      Water
   };

   class Texture2D;
   class ConstantBufferDX11;
   class MEAPI Material : public Resource
   {
   public:
      Material(ResourceManager* resMng);

      virtual bool Init(const String& filePath) override;
      virtual bool SaveTo(const String& filePath) override;

      void SetTexture2D(MaterialTextureProperty prop, Texture2D* texture);
      Texture2D* GetTexture2D(MaterialTextureProperty propertyType) const;

      void SetScalarFactor(MaterialFactorProperty prop, float factor);
      float GetScalarFactor(MaterialFactorProperty prop) const;
      void SetVector4Factor(MaterialFactorProperty prop, const Vector4& factor);
      Vector4 GetVector4Factor(MaterialFactorProperty prop) const;
      void SetVector2Factor(MaterialFactorProperty prop, const Vector2& factor);
      Vector2 GetVector2Factor(MaterialFactorProperty prop) const;

      void SetMaterialType(EMaterialType type) { m_materialType = type; }
      EMaterialType GetMaterialType() const { return m_materialType; }

      virtual json Serialize() const override;
      virtual void DeSerialize(const json& jsonData) override;

      void BindTextures(ID3D11DeviceContext& context, unsigned int bindSlot, EShaderType shaderType);
      void UnbindTextures(ID3D11DeviceContext& context, unsigned int boundSlot, EShaderType shaderType);
      void UpdateConstantBuffer(ID3D11DeviceContext& context, ConstantBufferDX11* buffer, float exposure = 1.0f) const;

   private:
      EMaterialType m_materialType;
      Texture2D* m_baseColor;
      Texture2D* m_emissive;
      Texture2D* m_metallicRoughness;
      Texture2D* m_ao;
      Texture2D* m_normal;

      Vector2  m_uvOffset;
      Vector4  m_baseColorFactor;
      float    m_emissiveFactor;
      float		m_metallicFactor;
      float		m_roughnessFactor;
      float    m_specularFactor;

   };
}