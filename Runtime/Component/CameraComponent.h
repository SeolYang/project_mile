#pragma once

#include "Component.h"
#include "Core\ComponentRegister.h"
#include "Math\Vector4.h"

namespace Mile
{
   class MEAPI CameraComponent : public Component
   {
      ComponentBegin( CameraComponent );
   public:
      CameraComponent( Entity* entity );

      float GetFov( ) const { return m_fov; }
      void SetFov( float fov ) { m_fov = fov; }

      float GetNearPlane( ) const { return m_nearPlane; }
      void SetNearPlane( float nearPlane ) { m_nearPlane = nearPlane; }

      float GetFarPlane( ) const { return m_farPlane; }
      void SetFarPlane( float farPlane ) { m_farPlane = farPlane; }

      Vector4 GetClearColor( ) const { return m_clearColor; }
      void SetClearColor( const Vector4& clearColor ) { m_clearColor = clearColor; }

   private:
      float m_fov;
      float m_nearPlane;
      float m_farPlane;

      Vector4 m_clearColor;

   };
}