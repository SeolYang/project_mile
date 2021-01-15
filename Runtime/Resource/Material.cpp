#include "Resource/Material.h"
#include "Resource/ResourceManager.h"
#include "Resource/Texture2D.h"
#include "Core/Context.h"
#include "Core/Logger.h"
#include "json.hpp"

namespace Mile
{
   Material::Material(Context* context, const String& filePath) :
      m_materialType(EMaterialType::Opaque),
      m_baseColor(nullptr),
      m_emissive(nullptr),
      m_metallicRoughness(nullptr),
      m_specularMap(nullptr),
      m_ao(nullptr),
      m_normal(nullptr),
      m_baseColorFactor(Vector4(0.0f, 0.0f, 0.0f, 1.0f)),
      m_emissiveFactor(Vector4(0.0f, 0.0f, 0.0f, 1.0f)),
      m_metallicFactor(0.0f),
      m_roughnessFactor(0.0f),
      m_uvOffset(Vector2(0.0f, 0.0f)),
      m_specularFactor(0.0f),
      Resource(context, filePath, ResourceType::Material)
   {
      SetTexture2D(MaterialTextureProperty::BaseColor, nullptr);
      SetTexture2D(MaterialTextureProperty::Emissive, nullptr);
      SetTexture2D(MaterialTextureProperty::MetallicRoughness, nullptr);
      SetTexture2D(MaterialTextureProperty::Specular, nullptr);
      SetTexture2D(MaterialTextureProperty::AO, nullptr);
      SetTexture2D(MaterialTextureProperty::Normal, nullptr);
   }

   bool Material::Init()
   {
      if (m_context == nullptr || m_bIsInitialized)
      {
         MELog(m_context, TEXT("Material"), ELogType::WARNING, TEXT("Already Initialized material."));
         return false;
      }

      std::ifstream stream(this->m_path);
      if (!stream.is_open())
      {
         MELog(m_context, TEXT("Material"), ELogType::WARNING, TEXT("Failed to load material from ") + m_path);
         return false;
      }

      std::string jsonStr;
      std::string temp;
      while (std::getline(stream, temp))
      {
         jsonStr += temp;
         jsonStr += '\n';
      }
      stream.close();

      this->DeSerialize(json::parse(jsonStr));
      return true;
   }

   void Material::SetTexture2D(MaterialTextureProperty prop, Texture2D* texture)
   {
      if (texture == nullptr)
      {
         assert(m_context != nullptr);
         ResourceManager* resMng = m_context->GetSubSystem<ResourceManager>();
         if (resMng != nullptr)
         {
            texture = resMng->Load<Texture2D>(TEXT("Contents/Textures/default_black.png"));
         }
         else
         {
            MELog(m_context, TEXT("Material::SetTexture2D"), ELogType::FATAL, TEXT("Couldn't load ResourceManager."));
         }
      }

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
      case MaterialTextureProperty::Specular:
         m_specularMap = texture;
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
      case MaterialTextureProperty::Specular:
         return m_specularMap;
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
      default:
         MELog(m_context, TEXT("Material::SetScalarFactor"), ELogType::WARNING, TEXT("Wrong property passed as scalar factor."));
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
      }

      MELog(m_context, TEXT("Material::GetScalarFactor"), ELogType::WARNING, TEXT("Couldn't find out property from scalar factors."));
      return 0.0f;
   }

   void Material::SetVector4Factor(MaterialFactorProperty prop, const Vector4& factor)
   {
      switch (prop)
      {
      case MaterialFactorProperty::BaseColor:
         m_baseColorFactor = factor;
         break;
      case MaterialFactorProperty::Emissive:
         m_emissiveFactor = factor;
         break;
      default:
         MELog(m_context, TEXT("Material::SetVector4Factor"), ELogType::WARNING, TEXT("Wrong property passed as Vector4 factor."));
         break;
      }
   }

   Vector4 Material::GetVector4Factor(MaterialFactorProperty prop) const
   {
      switch (prop)
      {
      case MaterialFactorProperty::BaseColor:
         return m_baseColorFactor;;
      case MaterialFactorProperty::Emissive:
         return m_emissiveFactor;
      }

      MELog(m_context, TEXT("Material::GetVector4Factor"), ELogType::WARNING, TEXT("Couldn't find out property from Vector4 factors."));
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
         MELog(m_context, TEXT("Material::SetVector4Factor"), ELogType::WARNING, TEXT("Wrong property passed as Vector4 factor."));
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

      MELog(m_context, TEXT("Material::GetVector2Factor"), ELogType::WARNING, TEXT("Couldn't find out property from Vector4 factors."));
      return Vector2(0.0f, 0.0f);
   }

   bool Material::SaveTo(const String& filePath)
   {
      json serialized = this->Serialize();
      std::ofstream stream(filePath);
      stream << serialized.dump();
      stream.close();

      return true;
   }

   json Material::Serialize() const
   {
      json serialized = Resource::Serialize();

      serialized["Type"] = static_cast<unsigned int>(m_materialType);

      serialized["BaseColor"] = WString2String(m_baseColor->GetPath());
      serialized["BaseColorFactor"] = m_baseColorFactor.Serialize();

      serialized["Emissive"] = WString2String(m_emissive->GetPath());
      serialized["EmissiveFactor"] = m_emissiveFactor.Serialize();

      serialized["MetallicRoughness"] = WString2String(m_metallicRoughness->GetPath());
      serialized["MetallicFactor"] = m_metallicFactor;
      serialized["RoughnessFactor"] = m_roughnessFactor;

      serialized["Specular"] = WString2String(m_specularMap->GetPath());
      serialized["SpecularFactor"] = m_specularFactor;

      serialized["UVOffset"] = m_uvOffset.Serialize();

      serialized["AO"] = WString2String(m_ao->GetPath());
      serialized["Normal"] = WString2String(m_normal->GetPath());

      return serialized;
   }

   void Material::DeSerialize(const json& jsonData)
   {
      assert(m_context != nullptr);
      ResourceManager * resMng = m_context->GetSubSystem<ResourceManager>();
      if (resMng == nullptr)
      {
         MELog(m_context, TEXT("Material"), ELogType::FATAL, TEXT("Couldn't load ResourceManager."));
         return;
      }

      SetTexture2D(
         MaterialTextureProperty::BaseColor,
         resMng->Load<Texture2D>(String2WString(GetValueSafelyFromJson<std::string>(jsonData, "BaseColor"))));

      m_baseColorFactor.DeSerialize(GetValueSafelyFromJson<json>(
         jsonData, 
         "BaseColorFactor",
         m_baseColorFactor.Serialize()));

      SetTexture2D(
         MaterialTextureProperty::Emissive,
         resMng->Load<Texture2D>(String2WString(GetValueSafelyFromJson<std::string>(jsonData, "Emissive"))));

      m_emissiveFactor.DeSerialize(GetValueSafelyFromJson<json>(
         jsonData,
         "EmissiveFactor",
         m_emissiveFactor.Serialize()));

      SetTexture2D(
         MaterialTextureProperty::MetallicRoughness,
         resMng->Load<Texture2D>(String2WString(GetValueSafelyFromJson<std::string>(jsonData, "MetallicRoughness"))));

      m_metallicFactor = GetValueSafelyFromJson(jsonData, "MetallicFactor", 0.0f);
      m_roughnessFactor = GetValueSafelyFromJson(jsonData, "RoughnessFactor", 0.0f);

      SetTexture2D(
         MaterialTextureProperty::Specular,
         resMng->Load<Texture2D>(String2WString(GetValueSafelyFromJson<std::string>(jsonData, "Specular"))));

      m_specularFactor = GetValueSafelyFromJson(jsonData, "SpecularFactor", 0.0f);

      m_uvOffset.DeSerialize(GetValueSafelyFromJson<json>(
         jsonData,
         "UVOffset",
         m_uvOffset.Serialize()));

      SetTexture2D(
         MaterialTextureProperty::AO,
         resMng->Load<Texture2D>(String2WString(GetValueSafelyFromJson<std::string>(jsonData, "AO"))));

      SetTexture2D(
         MaterialTextureProperty::Normal,
         resMng->Load<Texture2D>(String2WString(GetValueSafelyFromJson<std::string>(jsonData, "Normal"))));
   }
}