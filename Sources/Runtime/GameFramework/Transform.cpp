#include "GameFramework/Transform.h"
#include "GameFramework/Entity.h"

namespace Mile
{
   Transform* Transform::GetParent() const
   {
      Entity* parent = m_entity->GetParent();
      Transform* parentTrasnfrom = nullptr;
      if (parent != nullptr)
      {
         parentTrasnfrom = parent->GetTransform();
      }

      return parentTrasnfrom;
   }

   Vector3 Transform::GetForward(ETransformSpace space) const
   {
      Vector3 res = GetRotation(space).RotateVector(Vector3::Forward());
      res.Normalize();
      return res;
   }

   Vector3 Transform::GetUp(ETransformSpace space) const
   {
      Vector3 res = GetRotation(space).RotateVector(Vector3::Up());
      res.Normalize();
      return res;
   }
}