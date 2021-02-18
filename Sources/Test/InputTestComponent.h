#pragma once
#include "Core/CoreMinimal.h"
#include "Core/Engine.h"
#include "Core/InputManager.h"
#include "Core/Logger.h"
#include "Component/ComponentRegister.h"

using namespace Mile;

class InputTestComponent : public Component
{
   DeclareComponent(InputTestComponent);

public:
   InputTestComponent(Entity* entity) :
      Component(entity)
   {
   }

   void Forward()
   {
   }

   virtual void Start() override
   {
      auto inputManager = Engine::GetInputManager();
      if (inputManager != nullptr)
      {
         inputManager->BindAction(
            TEXT("Forward"),
            EInputEvent::Pressed,
            [&]() {
            this->Forward(); 
            });
      }
   }

   virtual json Serialize() const override
   {
      json serialized = Component::Serialize();
      serialized["Type"] = "InputTestComponent";
      return serialized;
   }
};
