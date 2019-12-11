#include "ComponentRegister.h"

namespace Mile
{
	ComponentRegister* ComponentRegister::m_instance = nullptr;

	Component* ComponentRegister::Acquire(const std::string& key, Entity* entity)
	{
		if (entity == nullptr)
		{
			return nullptr;
		}

		auto itr = m_constructorMap.find(key);
		if (itr == m_constructorMap.end())
		{
			return nullptr;
		}

		return (*itr).second(entity);
	}

	ComponentRegister& ComponentRegister::GetInstance()
	{
		if (ComponentRegister::m_instance == nullptr)
		{
			ComponentRegister::m_instance = new ComponentRegister();
		}

		return (*ComponentRegister::m_instance);
	}

	void ComponentRegister::Destroy()
	{
		SafeDelete(m_instance);
	}
}