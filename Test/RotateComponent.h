#pragma once

#include "CoreMinimal.h"

using namespace Mile;

class RotateComponent : public Component
{
   ComponentBegin( RotateComponent )

public:
   RotateComponent( Entity* entity ) :
      m_degree( 0.0f ),
      Component( entity )
   {
   }

   virtual void Update( )
   {
      auto timer = GetContext( )->GetSubSystem<Timer>( );
      auto transform = m_entity->GetTransform( );
      transform->Rotate( Quaternion( 180.0f * 0.2f * timer->GetDeltaTime( ), Vector3::Up( ) ) );
   }

};
