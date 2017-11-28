#pragma once

#include "Rendering.h"

namespace Mile
{
   enum class ShaderType
   {
      VertexShader,
      HullShader,
      DomainShader,
      GeometryShader,
      PixelShader
   };

   class Window;
   class DepthStencilBufferDX11;
   class MEAPI RendererDX11 : public SubSystem
   {
   public:
      RendererDX11( Context* context );
      virtual ~RendererDX11( );

      virtual bool Init( ) override;

      void Render( );

      void Clear( );
      void Present( );

      ID3D11Device* GetDevice( ) { return m_device; }
      ID3D11DeviceContext* GetDeviceContext( ) { return m_deviceContext; }

      void SetClearColor( Vector4 clearColor );
      Vector4 GetClearColor( ) const { return m_clearColor; }

      void SetDepthStencilEnable( bool bDepthEnabled );
      bool IsDepthStencilEnabled( ) const { return m_bDepthStencilEnabled; }

      void SetBackbufferAsRenderTarget( );

   private:
      bool CreateDeviceAndSwapChain( );
      bool CreateDepthStencilBuffer( );

   private:
      Window*                 m_window;
      ID3D11Device*           m_device;
      ID3D11DeviceContext*    m_deviceContext;

      // Back Buffer Variables
      IDXGISwapChain*         m_swapChain;
      ID3D11RenderTargetView* m_renderTargetView;
      std::unique_ptr<DepthStencilBufferDX11>   m_depthStencilBuffer;
      // \Back Buffer Variables

      bool                    m_bDepthStencilEnabled;
      Vector4                 m_clearColor;

   };
}