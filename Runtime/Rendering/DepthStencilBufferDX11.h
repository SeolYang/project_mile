#pragma once

#include "ResourceDX11.h"

namespace Mile
{
	/**
	 * @brief	Direct3D 11 의 깊이-스텐실 버퍼를 래핑하는 클래스 입니다.
	 */
   class MEAPI DepthStencilBufferDX11 : public ResourceDX11
   {
   public:
      DepthStencilBufferDX11( RendererDX11* renderer );
      ~DepthStencilBufferDX11( );

	  /**
	   * @brief	깊이-스텐실 버퍼를 초기화 합니다.
	   * @param width			깊이-스텐실 버퍼의 너비
	   * @param height			깊이-스텐실 버퍼의 높이
	   * @param stencilEnable	깊이-스텐실 버퍼에서 스텐실 버퍼의 활성화 여부
	   * @return	초기화 성공 여부
	   */
      bool Init( unsigned int width, unsigned int height, bool stencilEnable );

      virtual ID3D11Resource* GetResource( ) override { return m_depthStencilBuffer; }
      virtual RenderResourceType GetResourceType( ) const override { return RenderResourceType::RDRT_DepthStencilBuffer; }
	  /**
	   * @brief	깊이-스텐실 버퍼의 리소스 뷰를 반환 합니다.
	   * @return Direct3D Depth-stencil buffer resource view
	   */
      ID3D11DepthStencilView* GetDSV( ) { return m_depthStencilView; }

	  /**
	   * @brief	해당 버퍼가 스텐실이 활성화 된 상태로 초기화 되었는지 확인합니다.
	   * @return	스텐실 버퍼 활성화 여부
	   */
      bool IsStencilEnabled( ) const { return m_bStencilEnabled; }

   private:
      ID3D11Texture2D*        m_depthStencilBuffer;
      ID3D11DepthStencilView* m_depthStencilView;
      bool                    m_bStencilEnabled;

   };
}
