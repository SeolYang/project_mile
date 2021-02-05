#pragma once
#include "Rendering/RenderObject.h"

namespace Mile
{
   class Texture2dDX11;
   class DepthStencilBufferDX11;
   class MEAPI RenderTargetDX11 : public RenderObject
   {
   public:
      struct Descriptor
      {
         unsigned int Width = 1920;
         unsigned int Height = 1080;
         EColorFormat Format = EColorFormat::R8G8B8A8_UNORM;
         ID3D11RenderTargetView* RenderTargetView = nullptr;
         DepthStencilBufferDX11* DepthStencilBuffer = nullptr;
      };

   public:
      RenderTargetDX11(RendererDX11* renderer);
      virtual ~RenderTargetDX11();

      bool Init(const Descriptor& descriptor);
      bool Init(unsigned int width, unsigned int height, DXGI_FORMAT format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM, DepthStencilBufferDX11* depthStencilBuffer = nullptr);
      void DeInit();

      unsigned int GetWidth() const { return m_width; }
      unsigned int GetHeight() const { return m_height; }
      Texture2dDX11* GetTexture() const { return m_texture; }
      ID3D11RenderTargetView* GetRTV() const { return m_rtv; }

      bool BindAsRenderTarget(ID3D11DeviceContext& deviceContext, bool clearTarget = true, bool clearDepthStencil = true);
      bool BindAsShaderResource(ID3D11DeviceContext& deviceContext, unsigned int startSlot, EShaderType shader);
      void UnbindRenderTarget(ID3D11DeviceContext& deviceContext);
      void UnbindShaderResource(ID3D11DeviceContext& deviceContext);

      void SetDepthStencilBuffer(DepthStencilBufferDX11* buffer) { this->m_depthStencilBuffer = buffer; }
      void SetClearColor(const Vector4& color);
      void ClearDepthStencil(ID3D11DeviceContext& deviceContext);

      /**
       * @remark  이 함수를 통해 객체를 초기화 할 경우, 셰이더 리소스로는 바운드 시킬 수 없습니다. (항상 렌더 타겟으로만 Output Merge Stage 에 바인드 되어야 합니다.)
       */
      bool Init(ID3D11RenderTargetView* rtv, DepthStencilBufferDX11* depthStencilBuffer = nullptr);

   private:
      ID3D11RenderTargetView* m_rtv;
      Texture2dDX11* m_texture;
      DepthStencilBufferDX11* m_depthStencilBuffer; /** Does not delete in render target! */

      unsigned int               m_width;
      unsigned int               m_height;
      Vector4                    m_clearColor;

   };
}
