#include "Rendering/RasterizerState.h"
#include "Rendering/RendererDX11.h"

namespace Mile
{
   RasterizerState::RasterizerState(RendererDX11* renderer) :
      m_rasterizerState(nullptr),
      m_bIsDirty(true),
      m_bIsWireframe(false),
      m_cullMode(ECullMode::Back),
      m_windingOrder(EWindingOrder::CW),
      m_depthBias(0),
      m_slopeScaledDepthBias(0.0f),
      m_depthBiasClamp(0.0f),
      m_bIsDepthClipEnable(true),
      RenderObject(renderer)
   {
   }

   RasterizerState::~RasterizerState()
   {
      SafeRelease(m_rasterizerState);
   }

   bool RasterizerState::Init()
   {
      if (IsInitializable() || m_rasterizerState == nullptr)
      {
         D3D11_RASTERIZER_DESC desc;
         ZeroMemory(&desc, sizeof(desc));
         desc.FillMode = (m_bIsWireframe) ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;

         switch (m_cullMode)
         {
         case ECullMode::None:
            desc.CullMode = D3D11_CULL_NONE;
            break;

         case ECullMode::Back:
            desc.CullMode = D3D11_CULL_BACK;
            break;

         case ECullMode::Front:
            desc.CullMode = D3D11_CULL_FRONT;
            break;
         }

         desc.FrontCounterClockwise = (m_windingOrder == EWindingOrder::CCW);
         desc.DepthBias = m_depthBias;
         desc.SlopeScaledDepthBias = m_slopeScaledDepthBias;
         desc.DepthBiasClamp = m_depthBiasClamp;
         desc.DepthClipEnable = m_bIsDepthClipEnable;
         desc.ScissorEnable = false;
         desc.MultisampleEnable = false;
         desc.AntialiasedLineEnable = false;

         RendererDX11* renderer = GetRenderer();
         auto& device = renderer->GetDevice();
         auto result = device.CreateRasterizerState(&desc, &m_rasterizerState);
         if (FAILED(result))
         {
            m_bIsDirty = true;
            return false;
         }

         m_bIsDirty = false;
         RenderObject::ConfirmInit();
         return true;
      }

      return false;
   }

   bool RasterizerState::Bind(ID3D11DeviceContext& deviceContext)
   {
      if (IsBindable())
      {
         if (m_bIsDirty)
         {
            SafeRelease(m_rasterizerState);
            if (!Init())
            {
               return false;
            }
         }

         deviceContext.RSSetState(m_rasterizerState);
         return true;
      }

      return false;
   }
}