#include "RotateComponent.h"
#include "GameFramework/Entity.h"
#include "GameFramework/Transform.h"
#include "Core/Timer.h"
#include "Core/Engine.h"

DefineComponent(RotateComponent);

void RotateComponent::Update()
{
   auto timer = Engine::GetTimer();
   auto transform = m_entity->GetTransform();
   transform->Rotate(Quaternion(20.0f * timer->GetDeltaTime(), Vector3::Up()));
}