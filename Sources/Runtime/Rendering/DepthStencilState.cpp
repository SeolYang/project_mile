#include "Rendering/DepthStencilState.h"
#include "Rendering/RendererDX11.h"

namespace Mile
{
   DepthStencilState::DepthStencilState(RendererDX11* renderer) :
      m_bIsDirty(true),
      m_depthStencilState(nullptr),
      m_desc(GetDefaultDesc()),
      RenderObject(renderer)
   {
   }

   DepthStencilState::~DepthStencilState()
   {
      SafeRelease(m_depthStencilState);
   }

   bool DepthStencilState::Init()
   {
      if (IsInitializable() || m_depthStencilState == nullptr)
      {
         RendererDX11* renderer = GetRenderer();
         auto device = renderer->GetDevice();
         auto result = device->CreateDepthStencilState(&m_desc, &m_depthStencilState);
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

   bool DepthStencilState::Bind(ID3D11DeviceContext& deviceContext)
   {
      if (IsBindable())
      {
         if (m_bIsDirty)
         {
            SafeRelease(m_depthStencilState);
            if (!Init())
            {
               return false;
            }
         }

         deviceContext.OMSetDepthStencilState(m_depthStencilState, 0);
         return true;
      }

      return false;
   }
}