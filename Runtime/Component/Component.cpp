#include "Component/Component.h"
#include "GameFramework/Entity.h"

namespace Mile
{
   Component::Component(Entity* entity) :
      m_entity(entity),
      m_bIsActivated(true)
   {
   }

   bool Component::IsActivated() const
   {
      return (m_entity->IsActivated() && m_bIsActivated);
   }

   void Component::SetActive(bool bIsActive)
   {
      if (m_bIsActivated != bIsActive)
      {
         m_bIsActivated = bIsActive;
         if (m_bIsActivated)
         {
            OnEnable();
         }
         else
         {
            OnDisable();
         }
      }
   }

   Context* Component::GetContext() const
   {
      return m_entity->GetContext();
   }

   Transform* Component::GetTransform() const
   {
      return m_entity->GetTransform();
   }
}
