#include "Context.h"
#include "ComponentRegister.h"

namespace Mile
{
   Context::Context( )
   {
   }

   Context::~Context( )
   {
      for ( size_t idx = m_subSystems.size()-1; idx; --idx )
      {
         m_subSystems[ idx ]->DeInit( );
         SafeDelete( m_subSystems[ idx ] );
      }

      ComponentRegister::Destroy( );
   }

   void Context::RegisterSubSystem( SubSystem* newSubSystem )
   {
      if ( newSubSystem != nullptr )
      {
         m_subSystems.push_back( newSubSystem );
      }
   }
}