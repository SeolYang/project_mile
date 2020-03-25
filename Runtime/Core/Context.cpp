#include "Core/Context.h"
#include "Component/ComponentRegister.h"

namespace Mile
{
   Context::Context()
   {
   }

   Context::~Context()
   {
      for (auto itr = m_subSystems.end() - 1; itr != (m_subSystems.begin()); )
      {
         auto subSys = (*itr);
         SafeDelete(subSys);
         itr = m_subSystems.erase(itr) - 1;
      }

      // Finally, delete Engine subsystem
      SafeDelete(m_subSystems[0]);
      m_subSystems.clear();

      ComponentRegister::Destroy();
   }

   void Context::RegisterSubSystem(SubSystem* newSubSystem)
   {
      if (newSubSystem != nullptr)
      {
         m_subSystems.push_back(newSubSystem);
      }
   }
}