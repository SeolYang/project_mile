#pragma once
#include "Rendering/RenderingPass.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

namespace Mile
{
   /* @TODO:
   라이트를 한번에 shader 에서 처리
   vs
   느릴 수 있지만 라이트 마다 각각 light buffer pass 를 거치게 하여 라이트의 수를 유동적으로 컨트롤 가능하게 하기
   @comment Unreal Engine 의 경우 light type 에 따라 최대 라이트 수가 다르다(Static 한지 dynamic 한지..) => 라이트맵 baking 하게 되면 고려해보기
   */
   class GBuffer;
   class MEAPI LightBufferPass : public RenderingPass
   {
      struct LightParamConstantBuffer
      {
         alignas(16) Vector3 LightPos;
         alignas(16) Vector3 LightColor;
         alignas(16) Vector3 LightDirection;
         alignas(16) Vector2 SpotlightAngles;
         alignas(16) Vector3 LightRange;
         unsigned int LightType;
      };

      struct alignas(16) CameraConstantBuffer
      {
         Vector3 CameraPos;
      };

   public:
      LightBufferPass(RendererDX11* renderer);
      ~LightBufferPass();

      virtual bool Init(const String& filePath);
      virtual bool Bind(ID3D11DeviceContext& deviceContext) override;
      virtual void Unbind(ID3D11DeviceContext& deviceContext) override;

      void SetGBuffer(GBuffer* gBuffer);
      // CheckerBoard Setting does not change after decided at GBuffer-Pass
      void SetCheckerBoardBuffer(CBufferPtr checkerBoardBuffer) { m_checkerBoardBuffer = checkerBoardBuffer; }
      void SetLightBuffer(RenderTargetDX11* lightBuffer);

      void UpdateLightParamBuffer(ID3D11DeviceContext& deviceContext, 
         const Vector3& lightPos,
         const Vector3& lightColor,
         const Vector3& lightDirection,
         const Vector2& spotlightAngles,
         const Vector3& lightRange,
         unsigned int lightType);

      void UpdateCameraBuffer(ID3D11DeviceContext& deviceContext, const Vector3& camPos);

   private:
      /* Light Buffer pass Constant buffers */
      ConstantBufferDX11* m_lightParamBuffer;
      ConstantBufferDX11* m_cameraBuffer;

      /* External Constant Buffer*/
      ConstantBufferDX11* m_checkerBoardBuffer;

      GBuffer* m_gBuffer;
      RenderTargetDX11* m_lightBuffer;

   };
}