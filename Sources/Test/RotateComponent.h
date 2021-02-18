#pragma once
#include "Core/CoreMinimal.h"
#include "Component/ComponentRegister.h"

using namespace Mile;

class RotateComponent : public Component
{
   DeclareComponent(RotateComponent);

public:
   RotateComponent( Entity* entity ) :
      Component( entity )
   {
   }

   virtual void Update() override;

   virtual json Serialize() const override
   {
	   json serialized = Component::Serialize();
	   serialized["Type"] = "RotateComponent";
	   return serialized;
   }

};
