#include "Core/SubSystem.h"

namespace Mile
{
   SubSystem::SubSystem(Context* context) :
      m_bIsInitialized(false),
      m_context(context)
   {
   }

   SubSystem::~SubSystem()
   {
      DeInit();
   }
}