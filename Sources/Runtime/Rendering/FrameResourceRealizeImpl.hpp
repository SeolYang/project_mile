#pragma once
#include "Rendering/FrameResources.h"
#include "Rendering/RenderTargetDX11.h"
#include "Rendering/VertexShaderDX11.h"
#include "Rendering/PixelShaderDX11.h"
#include "Rendering/SamplerDX11.h"
#include "Rendering/DepthStencilBufferDX11.h"
#include "Rendering/ConstantBufferDX11.h"
#include "Rendering/GBuffer.h"
#include "Rendering/Viewport.h"
#include "Rendering/RasterizerState.h"
#include "Rendering/DepthStencilState.h"
#include "Rendering/Texture2dDX11.h"
#include "Rendering/Mesh.h"
#include "Rendering/Cube.h"
#include "Rendering/Quad.h"
#include "Rendering/DynamicCubemap.h"
#include "Core/Context.h"
#include "Core/Engine.h"
#include "GameFramework/World.h"
#include "GameFramework/Transform.h"
#include "Component/CameraComponent.h"
#include "Component/LightComponent.h"
#include "Component/MeshRenderComponent.h"
#include "Component/SkyboxComponent.h"
#include "Resource/ResourceManager.h"
#include "Resource/RenderTexture.h"
#include "Resource/Material.h"
#include "Resource/Texture2D.h"

namespace Elaina
{
   using namespace Mile;
   template<>
   CameraRef* Realize(const CameraRefDescriptor& descriptor)
   {
      return new CameraRef(descriptor.Reference);
   }

   template<>
   Lights* Realize(const WorldDescriptor& descriptor)
   {
      std::vector<LightComponent*>* lights = new std::vector<LightComponent*>();
      if (descriptor.TargetWorld != nullptr)
      {
         (*lights) = std::move(descriptor.TargetWorld->GetComponentsFromEntities<LightComponent>());
      }

      return lights;
   }

   template<>
   Meshes* Realize(const WorldDescriptor& descriptor)
   {
      std::vector<MeshRenderComponent*>* meshes = new std::vector<MeshRenderComponent*>();
      if (descriptor.TargetWorld != nullptr)
      {
         (*meshes) = std::move(descriptor.TargetWorld->GetComponentsFromEntities<MeshRenderComponent>());
      }

      return meshes;
   }

   template<>
   MaterialMap* Realize(const WorldDescriptor& descriptor)
   {
      auto materialMap = new MaterialMap();
      auto meshes = std::move(descriptor.TargetWorld->GetComponentsFromEntities<MeshRenderComponent>());
      for (auto renderComponent : meshes)
      {
         auto material = renderComponent->GetMaterial();
         if (material != nullptr)
         {
            (*materialMap)[material].push_back(renderComponent);
         }
      }

      return materialMap;
   }

   template<>
   RenderTargetDX11* Realize(const RenderTargetDescriptor& descriptor)
   {
      auto renderTarget = new RenderTargetDX11(descriptor.Renderer);
      bool bInitialized = false;
      if (descriptor.RenderTargetView != nullptr)
      {
         bInitialized = renderTarget->Init(descriptor.RenderTargetView, descriptor.DepthStencilBuffer);
      }
      else
      {
         bInitialized = renderTarget->Init(descriptor.Width, descriptor.Height, descriptor.Format, descriptor.DepthStencilBuffer);
      }

      if (!bInitialized)
      {
         Elaina::SafeDelete(renderTarget);
      }

      return renderTarget;
   }

   template<>
   RenderTargetRef* Realize(const RenderTargetRefDescriptor& desciptor)
   {
      return new RenderTargetRef(desciptor.Reference);
   }

   template<>
   DepthStencilBufferDX11* Realize(const DepthStencilBufferDescriptor& descriptor)
   {
      auto depthStencilBuffer = new DepthStencilBufferDX11(descriptor.Renderer);
      if (!depthStencilBuffer->Init(descriptor.Width, descriptor.Height, descriptor.bStencilEnable))
      {
         Elaina::SafeDelete(depthStencilBuffer);
      }

      return depthStencilBuffer;
   }

   template<>
   DepthStencilBufferRef* Realize(const DepthStencilBufferRefDescriptor& desc)
   {
      return new DepthStencilBufferRef(desc.Reference);
   }

   template<>
   VertexShaderDX11* Realize(const ShaderDescriptor& desc)
   {
      VertexShaderDX11* shader = new VertexShaderDX11(desc.Renderer);
      if (!shader->Init(desc.FilePath))
      {
         Elaina::SafeDelete(shader);
      }

      return shader;
   }

   template<>
   PixelShaderDX11* Realize(const ShaderDescriptor& desc)
   {
      PixelShaderDX11* shader = new PixelShaderDX11(desc.Renderer);
      if (!shader->Init(desc.FilePath))
      {
         Elaina::SafeDelete(shader);
      }

      return shader;
   }

   template<>
   SamplerDX11* Realize(const SamplerDescriptor& desc)
   {
      SamplerDX11* sampler = new SamplerDX11(desc.Renderer);
      if (!sampler->Init(desc.Filter, desc.AddressModeU, desc.AddressModeV, desc.AddressModeW, desc.CompFunc))
      {
         Elaina::SafeDelete(sampler);
      }

      return sampler;
   }

   template<>
   ConstantBufferDX11* Realize(const ConstantBufferDescriptor& desc)
   {
      ConstantBufferDX11* buffer = new ConstantBufferDX11(desc.Renderer);
      if (!buffer->Init(desc.Size))
      {
         Elaina::SafeDelete(buffer);
      }

      return buffer;
   }

   template<>
   GBuffer* Realize(const GBufferDescriptor& desc)
   {
      bool bInitialized = false;
      GBuffer* gBuffer = new GBuffer(desc.Renderer);
      if (desc.OutputRenderTargetReference == nullptr)
      {
         if (gBuffer->Init(desc.Width, desc.Height))
         {
            bInitialized = true;
         }
      }
      else if ((*desc.OutputRenderTargetReference) != nullptr)
      {
         if (gBuffer->Init(
            (*desc.OutputRenderTargetReference)->GetWidth(),
            (*desc.OutputRenderTargetReference)->GetHeight()))
         {
            bInitialized = true;
         }
      }

      if (!bInitialized)
      {
         Elaina::SafeDelete(gBuffer);
      }

      return gBuffer;
   }

   template<>
   GBufferRef* Realize(const GBufferRefDescriptor& desc)
   {
      return new GBufferRef(desc.Reference);
   }

   template<>
   Viewport* Realize(const ViewportDescriptor& desc)
   {
      Viewport* viewport = new Viewport(desc.Renderer);
      if (desc.OutputRenderTargetReference != nullptr)
      {
         if ((*desc.OutputRenderTargetReference) != nullptr)
         {
            viewport->SetWidth((*desc.OutputRenderTargetReference)->GetWidth());
            viewport->SetHeight((*desc.OutputRenderTargetReference)->GetHeight());
         }
      }
      else
      {
         viewport->SetWidth(desc.Width);
         viewport->SetHeight(desc.Height);
      }
      viewport->SetMinDepth(desc.MinDepth);
      viewport->SetMaxDepth(desc.MaxDepth);
      viewport->SetTopLeftX(desc.TopLeftX);
      viewport->SetTopLeftY(desc.TopLeftY);
      return viewport;
   }

   template<>
   RasterizerState* Realize(const RasterizerStateDescriptor& desc)
   {
      RasterizerState* state = new RasterizerState(desc.Renderer);
      state->SetWireframeRender(desc.bIsWireframe);
      state->SetCullMode(desc.CullMode);
      state->SetWindingOrder(desc.WindingOrder);
      state->SetDepthBias(desc.DepthBias);
      state->SetSlopeScaledDepthBias(desc.SlopeScaledDepthBias);
      state->SetDepthBias(desc.DepthBias);
      state->SetDepthClipEnable(desc.bIsDepthClipEnable);
      if (!state->Init())
      {
         Elaina::SafeDelete(state);
      }

      return state;
   }

   template<>
   Texture2DRef* Realize(const Texture2DRefDescriptor& desc)
   {
      Texture2DRef texture2D = desc.Reference;
      if (desc.Reference == nullptr)
      {
         texture2D = Engine::GetResourceManager()->GetByPath<Texture2D>(desc.ResourcePath);
      }

      return new Texture2DRef(texture2D);
   }

   template<>
   BoolRef* Realize(const BoolRefDescriptor& desc)
   {
      return new BoolRef(desc.Reference);
   }

   template<>
   UINT32Ref* Realize(const UINT32RefDescriptor& desc)
   {
      return new UINT32Ref(desc.Reference);
   }

   template<>
   Matrix* Realize(const MatrixDescriptor& desc)
   {
      return new Matrix(desc.Value);
   }

   template<>
   DepthStencilState* Realize(const DepthStencilStateDescriptor& desc)
   {
      DepthStencilState* state = new DepthStencilState(desc.Renderer);
      D3D11_DEPTH_STENCIL_DESC d3dDesc;
      d3dDesc.DepthEnable = desc.bDepthEnable;
      d3dDesc.DepthWriteMask = desc.DepthWriteMask;
      d3dDesc.DepthFunc = desc.DepthFunc;
      d3dDesc.StencilEnable = desc.bStencilEnable;
      d3dDesc.StencilReadMask = desc.StencilReadMask;
      d3dDesc.StencilWriteMask = desc.StencilWriteMask;
      d3dDesc.FrontFace = desc.FrontFace;
      d3dDesc.BackFace = desc.BackFace;
      state->SetDesc(d3dDesc);
      if (!state->Init())
      {
         Elaina::SafeDelete(state);
      }

      return state;
   }

   template<>
   MeshRef* Realize(const MeshRefDescriptor& desc)
   {
      return new MeshRef(desc.Reference);
   }

   template<>
   DynamicCubemap* Realize(const DynamicCubemapDescriptor& desc)
   {
      DynamicCubemap* map = new DynamicCubemap(desc.Renderer);
      if (!map->Init(desc.Size))
      {
         Elaina::SafeDelete(map);
      }

      return map;
   }

   template<>
   DynamicCubemapRef* Realize(const DynamicCubemapRefDescriptor& desc)
   {
      return new DynamicCubemapRef(desc.Reference);
   }
}