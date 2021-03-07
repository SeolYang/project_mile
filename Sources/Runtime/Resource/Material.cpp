#include "Resource/Material.h"
#include "Resource/ResourceManager.h"
#include "Resource/Texture2D.h"
#include "Rendering/Texture2dDX11.h"
#include "Rendering/ConstantBufferDX11.h"
#include "Core/Engine.h"

namespace Mile
{
   DEFINE_LOG_CATEGORY(MileMaterial);

   Material::Material(ResourceManager* resMng) :
      m_materialType(EMaterialType::Opaque),
      m_baseColor(nullptr),
      m_emissive(nullptr),
      m_metallicRoughness(nullptr),
      m_ao(nullptr),
      m_normal(nullptr),
      m_baseColorFactor(Vector4(0.0f, 0.0f, 0.0f, 1.0f)),
      m_emissiveFactor(0.0f),
      m_metallicFactor(0.0f),
      m_roughnessFactor(0.0f),
      m_uvOffset(Vector2(0.0f, 0.0f)),
      m_specularFactor(0.5f),
      Resource(resMng, ResourceType::Material)
   {
   }

   bool Material::Init(const String& filePath)
   {
      if (Resource::Init(filePath))
      {
         std::ifstream stream(this->m_path);
         if (!stream.is_open())
         {
            ME_LOG(MileMaterial, Warning, TEXT("Failed to open stream from ") + m_path);
         }

         std::string jsonStr;
         std::string temp;
         while (std::getline(stream, temp))
         {
            jsonStr += temp;
            jsonStr += '\n';
         }
         stream.close();

         if (!jsonStr.empty())
         {
            this->DeSerialize(json::parse(jsonStr));
            SucceedInit();
            return true;
         }
      }

      return false;
   }

   void Material::SetTexture2D(MaterialTextureProperty prop, Texture2D* texture)
   {
      switch (prop)
      {
      case MaterialTextureProperty::BaseColor:
         m_baseColor = texture;
         break;
      case MaterialTextureProperty::Emissive:
         m_emissive = texture;
         break;
      case MaterialTextureProperty::MetallicRoughness:
         m_metallicRoughness = texture;
         break;
         break;
      case MaterialTextureProperty::AO:
         m_ao = texture;
         break;
      case MaterialTextureProperty::Normal:
         m_normal = texture;
         break;
      }
   }

   Texture2D* Material::GetTexture2D(MaterialTextureProperty prop) const
   {
      switch (prop)
      {
      case MaterialTextureProperty::BaseColor:
         return m_baseColor;
      case MaterialTextureProperty::Emissive:
         return m_emissive;
      case MaterialTextureProperty::MetallicRoughness:
         return m_metallicRoughness;
      case MaterialTextureProperty::Normal:
         return m_normal;
      case MaterialTextureProperty::AO:
         return m_ao;
      }

      return nullptr;
   }

   void Material::SetScalarFactor(MaterialFactorProperty prop, float factor)
   {
      switch (prop)
      {
      case MaterialFactorProperty::Metallic:
         m_metallicFactor = factor;
         break;
      case MaterialFactorProperty::Roughness:
         m_roughnessFactor = factor;
         break;
      case MaterialFactorProperty::Specular:
         m_specularFactor = factor;
         break;
      case MaterialFactorProperty::Emissive:
         m_emissiveFactor = factor;
      default:
         ME_LOG(MileMaterial, Warning, TEXT("Wrong property passed as scalar factor."));
         break;
      }
   }

   float Material::GetScalarFactor(MaterialFactorProperty prop) const
   {
      switch (prop)
      {
      case MaterialFactorProperty::Metallic:
         return m_metallicFactor;
      case MaterialFactorProperty::Roughness:
         return m_roughnessFactor;
      case MaterialFactorProperty::Specular:
         return m_specularFactor;
      case MaterialFactorProperty::Emissive:
         return m_emissiveFactor;
      }

      ME_LOG(MileMaterial, Warning, TEXT("Couldn't find out property from scalar factors."));
      return 0.0f;
   }

   void Material::SetVector4Factor(MaterialFactorProperty prop, const Vector4& factor)
   {
      switch (prop)
      {
      case MaterialFactorProperty::BaseColor:
         m_baseColorFactor = factor;
         break;
      default:
         ME_LOG(MileMaterial, Warning, TEXT("Wrong property passed as Vector4 factor."));
         break;
      }
   }

   Vector4 Material::GetVector4Factor(MaterialFactorProperty prop) const
   {
      switch (prop)
      {
      case MaterialFactorProperty::BaseColor:
         return m_baseColorFactor;;
      }

      ME_LOG(MileMaterial, Warning, TEXT("Couldn't find out property from Vector4 factors."));
      return Vector4(0.0f, 0.0f, 0.0f, 1.0f);
   }

   void Material::SetVector2Factor(MaterialFactorProperty prop, const Vector2& factor)
   {
      switch (prop)
      {
      case MaterialFactorProperty::UVOffset:
         m_uvOffset = factor;
         break;
      default:
         ME_LOG(MileMaterial, Warning, TEXT("Wrong property passed as Vector4 factor."));
         break;
      }
   }

   Vector2 Material::GetVector2Factor(MaterialFactorProperty prop) const
   {
      switch (prop)
      {
      case MaterialFactorProperty::UVOffset:
         return m_uvOffset;
      }

      ME_LOG(MileMaterial, Warning, TEXT("Couldn't find out property from Vector4 factors."));
      return Vector2(0.0f, 0.0f);
   }

   bool Material::SaveTo(const String& filePath)
   {
      if (Resource::SaveTo(filePath))
      {
         json serialized = this->Serialize();
         std::ofstream stream(filePath);
         stream << serialized.dump();
         stream.close();

         return true;
      }

      return false;
   }

   json Material::Serialize() const
   {
      json serialized = Resource::Serialize();

      serialized["Type"] = static_cast<unsigned int>(m_materialType);

      if (m_baseColor != nullptr)
      {
         serialized["BaseColor"] = WString2String(m_baseColor->GetPath());
      }
      serialized["BaseColorFactor"] = m_baseColorFactor.Serialize();

      if (m_emissive != nullptr)
      {
         serialized["Emissive"] = WString2String(m_emissive->GetPath());
      }
      serialized["EmissiveFactor"] = m_emissiveFactor;

      if (m_metallicRoughness != nullptr)
      {
         serialized["MetallicRoughness"] = WString2String(m_metallicRoughness->GetPath());
      }
      serialized["MetallicFactor"] = m_metallicFactor;
      serialized["RoughnessFactor"] = m_roughnessFactor;

      serialized["SpecularFactor"] = m_specularFactor;

      serialized["UVOffset"] = m_uvOffset.Serialize();

      if (m_ao != nullptr)
      {
         serialized["AO"] = WString2String(m_ao->GetPath());
      }

      if (m_normal != nullptr)
      {
         serialized["Normal"] = WString2String(m_normal->GetPath());
      }

      return serialized;
   }

   void Material::DeSerialize(const json& jsonData)
   {
      assert(m_resMng != nullptr);
      if (m_resMng == nullptr)
      {
         ME_LOG(MileMaterial, Fatal, TEXT("ResourceManager does not exist!"));
         return;
      }

      SetTexture2D(
         MaterialTextureProperty::BaseColor,
         m_resMng->Load<Texture2D>(String2WString(GetValueSafelyFromJson<std::string>(jsonData, "BaseColor"))));

      m_baseColorFactor.DeSerialize(GetValueSafelyFromJson<json>(
         jsonData, 
         "BaseColorFactor",
         m_baseColorFactor.Serialize()));

      SetTexture2D(
         MaterialTextureProperty::Emissive,
         m_resMng->Load<Texture2D>(String2WString(GetValueSafelyFromJson<std::string>(jsonData, "Emissive"))));

      m_emissiveFactor = GetValueSafelyFromJson<json>(
         jsonData,
         "EmissiveFactor", 0.0f);

      SetTexture2D(
         MaterialTextureProperty::MetallicRoughness,
         m_resMng->Load<Texture2D>(String2WString(GetValueSafelyFromJson<std::string>(jsonData, "MetallicRoughness"))));

      m_metallicFactor = GetValueSafelyFromJson(jsonData, "MetallicFactor", 0.0f);
      m_roughnessFactor = GetValueSafelyFromJson(jsonData, "RoughnessFactor", 0.0f);

      m_specularFactor = GetValueSafelyFromJson(jsonData, "SpecularFactor", 0.5f);

      m_uvOffset.DeSerialize(GetValueSafelyFromJson<json>(
         jsonData,
         "UVOffset",
         m_uvOffset.Serialize()));

      SetTexture2D(
         MaterialTextureProperty::AO,
         m_resMng->Load<Texture2D>(String2WString(GetValueSafelyFromJson<std::string>(jsonData, "AO"))));

      SetTexture2D(
         MaterialTextureProperty::Normal,
         m_resMng->Load<Texture2D>(String2WString(GetValueSafelyFromJson<std::string>(jsonData, "Normal"))));
   }

   void Material::BindTextures(ID3D11DeviceContext& context, unsigned int bindSlot, EShaderType shaderType)
   {
      SAFE_SHADER_RESOURCE_VIEW_BIND(m_baseColor == nullptr ? nullptr : m_baseColor->GetRawTexture(), context, 0, shaderType);
      SAFE_SHADER_RESOURCE_VIEW_BIND(m_emissive == nullptr ? nullptr : m_emissive->GetRawTexture(), context, 1, shaderType);
      SAFE_SHADER_RESOURCE_VIEW_BIND(m_metallicRoughness == nullptr ? nullptr : m_metallicRoughness->GetRawTexture(), context, 2, shaderType);
      SAFE_SHADER_RESOURCE_VIEW_BIND(m_ao == nullptr ? nullptr : m_ao->GetRawTexture(), context, 3, shaderType);
      SAFE_SHADER_RESOURCE_VIEW_BIND(m_normal == nullptr ? nullptr : m_normal->GetRawTexture(), context, 4, shaderType);
   }

   void Material::UnbindTextures(ID3D11DeviceContext& context, unsigned int boundSlot, EShaderType shaderType)
   {
      SAFE_SHADER_RESOURCE_VIEW_UNBIND(m_baseColor == nullptr ? nullptr : m_baseColor->GetRawTexture(), context, 0, shaderType);
      SAFE_SHADER_RESOURCE_VIEW_UNBIND(m_emissive == nullptr ? nullptr : m_emissive->GetRawTexture(), context, 1, shaderType);
      SAFE_SHADER_RESOURCE_VIEW_UNBIND(m_metallicRoughness == nullptr ? nullptr : m_metallicRoughness->GetRawTexture(), context, 2, shaderType);
      SAFE_SHADER_RESOURCE_VIEW_UNBIND(m_ao == nullptr ? nullptr : m_ao->GetRawTexture(), context, 3, shaderType);
      SAFE_SHADER_RESOURCE_VIEW_UNBIND(m_normal == nullptr ? nullptr : m_normal->GetRawTexture(), context, 4, shaderType);
   }

   void Material::UpdateConstantBuffer(ID3D11DeviceContext& context, ConstantBufferDX11* buffer, float exposure) const
   {
      auto materialParamsBuffer = buffer->Map<PackedMaterialParams>(context);
      materialParamsBuffer->BaseColorFactor = m_baseColorFactor;
      materialParamsBuffer->MetallicRoughnessUV = Vector4(m_metallicFactor, m_roughnessFactor, m_uvOffset.x, m_uvOffset.y);
      materialParamsBuffer->SpecularFactor = m_specularFactor;
      materialParamsBuffer->EmissiveFactor = m_emissiveFactor* exposure;
      buffer->UnMap(context);
   }
}