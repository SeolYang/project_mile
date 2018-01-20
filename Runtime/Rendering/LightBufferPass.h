#pragma once

#include "RenderingPass.h"
#include "Math\Vector2.h"
#include "Math\Vector3.h"
#include "Math\Vector4.h"

namespace Mile
{
   class GBuffer;
   class MEAPI LightBufferPass : public RenderingPass
   {
      struct LightParamConstantBuffer
      {
         alignas( 16 ) Vector3 LightPos;
         alignas( 16 ) Vector3 LightColor;
         alignas( 16 ) Vector3 LightDirection;
         alignas( 16 ) Vector2 SpotlightAngles;
         alignas( 16 ) Vector3 LightRange;
         unsigned int LightType;
      };

      struct alignas( 16 ) CameraConstantBuffer
      {
         Vector3 CameraPos;
      };

   public:
      LightBufferPass( RendererDX11* renderer );
      ~LightBufferPass( );

      virtual bool Init( const String& filePath );
      bool Bind( );
      virtual void Unbind( ) override;

      void SetGBuffer( GBuffer* gBuffer );
      void SetLightBuffer( RenderTargetDX11* lightBuffer );

      void UpdateLightParamBuffer( const Vector3& lightPos, 
                              const Vector3& lightColor,
                              const Vector3& lightDirection,
                              const Vector2& spotlightAngles,
                              const Vector3& lightRange,
                              unsigned int lightType );

      void UpdateCameraBuffer( const Vector3& camPos );

   private:
      ConstantBufferDX11*  m_lightParamBuffer;
      ConstantBufferDX11*  m_cameraBuffer;
      
      GBuffer*             m_gBuffer;
      RenderTargetDX11*    m_lightBuffer;

   };
}