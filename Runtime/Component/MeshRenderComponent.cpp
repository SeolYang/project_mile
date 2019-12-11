#include "MeshRenderComponent.h"
#include "Core\Entity.h"
#include "Core\Context.h"
#include "Resource\ResourceManager.h"
#include "Resource\Model.h"
#include "Resource\Material.h"
#include "Rendering\Mesh.h"

namespace Mile
{
	RegisterComponent(MeshRenderComponent)

	//std::string MeshRenderComponent::Serialize() const
	//{
	//	// { "Mesh": { "Model": (Model filepath), "Name": (Mesh Name) },
	//	// "Material": (Material filepath) }
	//	std::string res = "{ \"Type\": \"MeshRenderComponent\", "
	//		+ Component::Serialize() +
	//		", \"Mesh\": { \"Model\": \"" + WString2String(m_mesh->GetModelPath()) +
	//		"\", \"Name\": \"" + WString2String(m_mesh->GetName()) + "\" }, "
	//		" \"Material\": \"" + WString2String(m_material->GetPath()) + "\" }";
	//	return res;
	//}

	json MeshRenderComponent::Serialize() const
	{
		json serialized = Component::Serialize();
		serialized["Type"] = "MeshRenderComponent";

		json meshData;
		meshData["Model"] = WString2String(m_mesh->GetModelPath());
		meshData["Name"] = WString2String(m_mesh->GetName());

		serialized["Mesh"] = meshData;
		serialized["Material"] = WString2String(m_material->GetPath());

		return serialized;
	}

	void MeshRenderComponent::DeSerialize(const json& jsonData)
	{
		Component::DeSerialize(jsonData);
		auto resMng = m_entity->GetContext()->GetSubSystem<ResourceManager>();
		json meshData = jsonData["Mesh"];
		auto loadedModel = resMng->Load<Model>(String2WString(meshData["Model"]));
		if (loadedModel != nullptr)
		{
			m_mesh = loadedModel->GetMeshByName(String2WString(meshData["Name"]));
			m_material = resMng->Load<Material>(String2WString(jsonData["Material"]));
		}
	}
}