#include "CameraComponent.h"

namespace Mile
{
   RegisterComponent( CameraComponent )

   CameraComponent::CameraComponent( Entity* entity ) :
      m_fov( 45.0f ),
      m_nearPlane( 1.0f ),
      m_farPlane( 500.0f ),
      m_clearColor( Vector4( 0.133f, 0.137f, 0.15f, 1.0f ) ),
      Component( entity )
   {
   }
}