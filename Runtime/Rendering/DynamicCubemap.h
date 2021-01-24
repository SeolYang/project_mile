#pragma once
#include "Rendering/Texture2DBaseDX11.h"
#include <array>

namespace Mile
{
   /**
    * @brief   Dynamic Cubemap은 렌더타겟(GPU Readable/Writable)으로 쓰일수있는 큐브맵입니다.
    *          Face Index: (0, +X), (1, -X), (2, +Y), (3, -Y), (4, +Z), (5, -Z)
    * @format  (R16, G16, B16, A16)_UNORM
    *          
    */
   class DepthStencilBufferDX11;
   class MEAPI DynamicCubemap : public Texture2DBaseDX11
   {
   public:
      DynamicCubemap(RendererDX11* renderer);
      virtual ~DynamicCubemap();

      bool Init(unsigned int size);
      virtual ERenderResourceType GetResourceType() const override { return ERenderResourceType::DynamicCubemap; }

      bool BindAsRenderTarget(ID3D11DeviceContext& deviceContext, unsigned int faceIdx, unsigned int mipLevel = 0, bool clearRenderTarget = true, bool clearDepth = true);
      /* 
      * @warn  실제로 Cubemap이 렌더 타겟으로 바인드 되어있는지 여부를 따지지 않고 무조건 unbind 합니다.
      **/
      void UnbindAsRenderTarget(ID3D11DeviceContext& deviceContext);

      unsigned int GetMaxMipLevels() const { return m_maxMipLevels; }

   private:
      std::array<std::vector<ID3D11RenderTargetView*>, CUBE_FACES> m_rtvs;
      std::vector<DepthStencilBufferDX11*> m_depthStencils;
      unsigned int m_maxMipLevels;

   };
}
