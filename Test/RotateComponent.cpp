#include "RotateComponent.h"
#include "GameFramework/Entity.h"
#include "GameFramework/Transform.h"

RegisterComponent(RotateComponent);

void RotateComponent::Update()
{
   auto timer = GetContext()->GetSubSystem<Timer>();
   auto transform = m_entity->GetTransform();
   transform->Rotate(Quaternion(180.0f * 0.2f * timer->GetDeltaTime(), Vector3::Up()));
}