#pragma once
#include "Rendering/ResourceDX11.h"

namespace Mile
{
   /**
    * @brief	Direct3D 11 의 깊이-스텐실 버퍼를 래핑하는 클래스 입니다.
    * @TODO    DSV도 SRV로 활용 할 수 있도록 만들기
    */
   class MEAPI DepthStencilBufferDX11 : public ResourceDX11
   {
   public:
      DepthStencilBufferDX11(RendererDX11* renderer);
      virtual ~DepthStencilBufferDX11();

      /**
       * @brief	깊이-스텐실 버퍼를 초기화 합니다.
       * @param width			깊이-스텐실 버퍼의 너비
       * @param height			깊이-스텐실 버퍼의 높이
       * @param stencilEnable	깊이-스텐실 버퍼에서 스텐실 버퍼의 활성화 여부
       * @return	초기화 성공 여부
       */
      bool Init(unsigned int width, unsigned int height, bool stencilEnable);

      virtual ID3D11Resource* GetResource() const override { return m_depthStencilBuffer; }
      virtual ERenderResourceType GetResourceType() const override { return ERenderResourceType::DepthStencilBuffer; }
      /**
       * @brief	깊이-스텐실 버퍼의 리소스 뷰를 반환 합니다.
       * @return Direct3D Depth-stencil buffer resource view
       */
      ID3D11DepthStencilView* GetDSV() const { return m_depthStencilView; }
      ID3D11ShaderResourceView* GetSRV() const { return m_srv; }

      /**
       * @brief	해당 버퍼가 스텐실이 활성화 된 상태로 초기화 되었는지 확인합니다.
       * @return	스텐실 버퍼 활성화 여부
       */
      bool IsStencilEnabled() const { return m_bStencilEnabled; }

      bool BindAsShaderResource(ID3D11DeviceContext& deviceContext, unsigned int bindSlot, EShaderType bindShader);
      void UnbindShaderResource(ID3D11DeviceContext& deviceContext, unsigned int boundSlot, EShaderType boundShader);

   private:
      ID3D11Texture2D*        m_depthStencilBuffer;
      ID3D11DepthStencilView* m_depthStencilView;
      ID3D11ShaderResourceView* m_srv;
      bool m_bStencilEnabled;

   };
}
