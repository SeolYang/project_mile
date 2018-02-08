#pragma once

#include "RenderingPass.h"

namespace Mile
{
   class MEAPI CheckerBoardInterpolatePass : public RenderingPass
   {
      struct RenderTargetInfoBuffer
      {
         alignas( 16 ) int Width;
         alignas( 16 ) int Height;
      };

   public:
      CheckerBoardInterpolatePass( RendererDX11* renderer );
      ~CheckerBoardInterpolatePass( );

      virtual bool Init( const String& filePath ) override;
      virtual bool Bind( ID3D11DeviceContext& deviceContext ) override;
      virtual void Unbind( ID3D11DeviceContext& deviceContext )  override;

      void SetRenderTarget( RenderTargetDX11* renderTarget ) { m_renderTarget = renderTarget; }
      void SetCheckerBoard( RenderTargetDX11* checkerBoard ) { m_checkerBoard = checkerBoard; }
      void UpdateRenderTargetInfoBuffer( ID3D11DeviceContext& deviceContext, int width, int height );

   private:
      /* Constant Buffers */
      CBufferPtr     m_renderTargetInfoBuffer;

      /* Render Targets */
      RenderTargetDX11* m_renderTarget;
      RenderTargetDX11* m_checkerBoard;

   };
}