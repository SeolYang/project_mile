#pragma once

#include "Core\Helper.h"
#include "Component\Component.h"

namespace Mile
{
   class MEAPI ComponentRegister
   {
   public:
      template < typename Ty >
      void Register( const std::string& key )
      {
         m_constructorMap.insert( std::make_pair( key, &Component::Create<Ty> ) );
      }

      Component* Acquire( const std::string& key, Entity* entity )
      {
         auto itr = m_constructorMap.find( key );
         if ( itr == m_constructorMap.end( ) )
         {
            return nullptr;
         }

         return ( *itr ).second( entity );
      }

      static ComponentRegister& GetInstance( )
      {
         if ( m_instance == nullptr )
         {
            m_instance = new ComponentRegister( );
         }

         return ( *m_instance );
      }

      static void Destroy( )
      {
         SafeDelete( m_instance );
      }

   private:
      ComponentRegister( ) { }

   private:
      static ComponentRegister*  m_instance;
      std::map<std::string, std::function<Component*( Entity* )>> m_constructorMap;

   };
}

#define RegisteComponent(TYPE) private: TYPE _registeryInst; \
   TYPE() { Mile::ComponentRegister::GetInstance().Register<TYPE>( #TYPE ); }
