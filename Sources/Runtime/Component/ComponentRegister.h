#pragma once
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

#define DeclareComponent(TYPE) private: static TYPE _registeryInst; static wchar_t* _type; \
   TYPE() : TYPE(nullptr) { Mile::ComponentRegister::GetInstance().Register<TYPE>( #TYPE ); } \
   public: virtual String GetType() const { return String(TYPE::_type); }

#define DefineComponent(TYPE) TYPE TYPE::_registeryInst; wchar_t* TYPE::_type = TEXT(#TYPE);