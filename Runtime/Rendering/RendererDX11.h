#pragma once

#include "../Core/SubSystem.h"
#include "Math/Vector4.h"
#include <d3d11.h>

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

      void SetDepthEnable( bool bDepthEnabled );
      bool IsDepthEnabled( ) const { return m_bDepthEnabled; }

      void SetBackbufferAsRenderTarget( );
   
   private:
      bool CreateDeviceAndSwapChain( );
      bool CreateDepthStencilBuffer( );
      bool CreateDepthStencilView( );

   private:
      Window*                 m_window;
      ID3D11Device*           m_device;
      ID3D11DeviceContext*    m_deviceContext;
      IDXGISwapChain*         m_swapChain;
      ID3D11RenderTargetView* m_renderTargetView;

      ID3D11Texture2D*        m_depthStencilBuffer;
      ID3D11DepthStencilView* m_depthStencilView;
      bool                    m_bDepthEnabled;

      Vector4                 m_clearColor;
   };
}