#pragma once
#include "Core/Helper.h"
#include "Component/Component.h"

namespace Mile
{
   class MEAPI ComponentRegister
   {
   public:
      template < typename Ty >
      void Register(const std::string& key)
      {
         m_constructorMap.insert(std::make_pair(key, &Component::Create<Ty>));
      }

      Component* Acquire(const std::string& key, Entity* entity);
      static ComponentRegister& GetInstance();
      static void Destroy();

   private:
      ComponentRegister() { }

   private:
      static ComponentRegister* m_instance;
      std::map<std::string, std::function<Component* (Entity*)>> m_constructorMap;

   };
}

#define RegisterComponent(TYPE) TYPE TYPE::_registeryInst; String TYPE::_type = TEXT(#TYPE)
#define ComponentBegin(TYPE) private: static TYPE _registeryInst; static String _type; \
   TYPE() : TYPE(nullptr) { Mile::ComponentRegister::GetInstance().Register<TYPE>( #TYPE ); } \
   public: virtual String GetType() const { return TYPE::_type; }
