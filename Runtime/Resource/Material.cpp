#include "Material.h"
#include "Core\Context.h"
#include "Core\Logger.h"
#include "ResourceManager.h"
#include "Texture2D.h"
#include "json.hpp"

namespace Mile
{
	bool Material::Init()
	{
		if (m_context == nullptr || m_bIsInitialized)
		{
			MELog(m_context, TEXT("Material"), ELogType::WARNING, TEXT("Already Initialized material."), true);
			return false;
		}

		std::ifstream stream(this->m_path);
		if (!stream.is_open())
		{
			MELog(m_context, TEXT("Material"), ELogType::WARNING, TEXT("Failed to load material from ") + m_path, true);
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
		std::string diffuseMapPath = "Contents/Textures/default_white.png";
		if (m_diffuseMap != nullptr)
		{
			diffuseMapPath = WString2String(m_diffuseMap->GetPath());
		}

		std::string normalMapPath = "Contents/Textures/default_black.png";
		if (m_normalMap != nullptr)
		{
			normalMapPath = WString2String(m_normalMap->GetPath());
		}

		json serialized = Resource::Serialize();
		serialized["SpecExp"] = m_specExp;
		serialized["SpecAlbedo"] = m_specAlbedo.Serialize();
		serialized["DiffuseMap"] = diffuseMapPath;
		serialized["NormalMap"] = normalMapPath;

		return serialized;
	}

	void Material::DeSerialize(const json& jsonData)
	{
		auto resourceMng = m_context->GetSubSystem<ResourceManager>();
		m_specExp = jsonData["SpecExp"];
		m_specAlbedo.DeSerialize(jsonData["SpecAlbedo"]);
		m_diffuseMap = resourceMng->Load<Texture2D>(String2WString(jsonData["DiffuseMap"]));
		m_normalMap = resourceMng->Load<Texture2D>(String2WString(jsonData["NormalMap"]));

		if (m_diffuseMap == nullptr)
		{
			m_diffuseMap = resourceMng->Load<Texture2D>(TEXT("Contents/Textures/default_white.png"));
		}

		if (m_normalMap == nullptr)
		{
			m_normalMap = resourceMng->Load<Texture2D>(TEXT("Contents/Textures/default_black.png"));
		}
	}
}