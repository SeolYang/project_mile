#pragma once

#include "CoreMinimal.h"

using namespace Mile;

class RotateComponent : public Component
{
   ComponentBegin( RotateComponent )

public:
   RotateComponent( Entity* entity ) :
      Component( entity )
   {
   }

   virtual void Update( )
   {
      auto timer = GetContext( )->GetSubSystem<Timer>( );
      auto transform = m_entity->GetTransform( );
      auto rot = Quaternion( Math::Pi * 0.3f * timer->GetDeltaTime( ), Vector3(1, 0, 0 ) );
      transform->Rotate( rot );
   }
};
