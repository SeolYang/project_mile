#include "Rendering/RendererPBR.h"
#include "Rendering/RenderTargetDX11.h"
#include "Rendering/VertexShaderDX11.h"
#include "Rendering/PixelShaderDX11.h"
#include "Rendering/SamplerDX11.h"
#include "Rendering/ConstantBufferDX11.h"
#include "Rendering/GBuffer.h"
#include "Rendering/Viewport.h"
#include "Rendering/RasterizerState.h"
#include "Rendering/Texture2dDX11.h"
#include "Rendering/Mesh.h"
#include "Core/Context.h"
#include "Core/Engine.h"
#include "GameFramework/World.h"
#include "GameFramework/Transform.h"
#include "Component/CameraComponent.h"
#include "Component/LightComponent.h"
#include "Component/MeshRenderComponent.h"
#include "Resource/ResourceManager.h"
#include "Resource/RenderTexture.h"
#include "Resource/Material.h"
#include "Resource/Texture2D.h"
#include "MT/ThreadPool.h"

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
}

namespace Mile
{
   DEFINE_LOG_CATEGORY(MileRendererPBR);

   DEFINE_CONSTANT_BUFFER(GeometryPassTransformBuffer)
   {
      Matrix WorldMatrix;
      Matrix WorldViewMatrix;
      Matrix WorldViewProjMatrix;
   };

   DEFINE_CONSTANT_BUFFER(PackedMaterialParams)
   {
      Vector4 BaseColorFactor;
      Vector4 EmissiveColorFactor;
      Vector4 MetallicRoughnessUV;
      float SpecularFactor;
   };

   RendererPBR::RendererPBR(Context* context, size_t maximumThreads) :
      RendererDX11(context, maximumThreads),
      m_targetCamera(nullptr),
      m_outputRenderTarget(nullptr),
      m_geometryPassVS(nullptr),
      m_geometryPassPS(nullptr)
   {
   }

   RendererPBR::~RendererPBR()
   {
      SafeDelete(m_geometryPassPS);
      SafeDelete(m_geometryPassVS);
      m_frameGraph.Clear();
   }

   bool RendererPBR::Init(Window& window)
   {
      if (RendererDX11::Init(window))
      {
         if (InitShader())
         {
            if (InitFrameGraph())
            {
               return true;
            }
         }
      }

      ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to initialize PBR Renderer!"));
      return false;
   }

   void RendererPBR::RenderImpl(const World& world)
   {
      auto threadPool = Engine::GetThreadPool();

      m_targetCamera = nullptr;
      m_outputRenderTarget = nullptr;

      auto acquireMeshRenderersAndMatTask = threadPool->AddTask([&]()
         {
            m_meshes.clear();
            m_materialMap.clear();

            m_meshes = std::move(world.GetComponentsFromEntities<MeshRenderComponent>());
            for (auto renderComponent : m_meshes)
            {
               auto material = renderComponent->GetMaterial();
               if (material != nullptr)
               {
                  m_materialMap[material].push_back(renderComponent);
               }
            }
         });
      auto acquireLightsTask = threadPool->AddTask([&]()
         {
            m_lights.clear();
            m_lights = std::move(world.GetComponentsFromEntities<LightComponent>());
         });
      auto acquireCamerasTask = threadPool->AddTask([&]()
         {
            m_cameras.clear();
            m_cameras = std::move(world.GetComponentsFromEntities<CameraComponent>());
         });

      acquireMeshRenderersAndMatTask.get();
      acquireLightsTask.get();
      acquireCamerasTask.get();

      for (auto camera : m_cameras)
      {
         m_targetCamera = camera;
         RenderTexture* renderTexture = m_targetCamera->GetRenderTexture();
         if (renderTexture != nullptr)
         {
            m_outputRenderTarget = renderTexture->GetRenderTarget();
         }
         else
         {
#ifdef MILE_EDITOR
            ResourceManager* resManager = Engine::GetResourceManager();
            renderTexture = resManager->Load<RenderTexture>(EDITOR_GAME_VIEW_RENDER_TEXTURE, true);
            m_outputRenderTarget = renderTexture->GetRenderTarget();
#else
            m_outputRenderTarget = &GetBackBuffer();
#endif
         }

         m_frameGraph.Execute();
      }
   }

   bool RendererPBR::InitShader()
   {
      /** Geometry Pass Shaders */
      m_geometryPassVS = new VertexShaderDX11(this);
      if (!m_geometryPassVS->Init(TEXT("Contents/Shaders/GeometryPass.hlsl")))
      {
         ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to load geometry pass vertex shader!"));
      }

      m_geometryPassPS = new PixelShaderDX11(this);
      if (!m_geometryPassPS->Init(TEXT("Contents/Shaders/GeometryPass.hlsl")))
      {
         ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to load geometry pass pixel shader!"));
      }

      return true;
   }

   bool RendererPBR::InitFrameGraph()
   {
      auto targetCameraRefRes = m_frameGraph.AddExternalPermanentResource("CameraRef", CameraRefDescriptor(), &m_targetCamera);
      auto lightsInfoRes = m_frameGraph.AddExternalPermanentResource("Lights", WorldDescriptor(), &m_lights);
      auto meshesInfoRes = m_frameGraph.AddExternalPermanentResource("Meshes", WorldDescriptor(), &m_meshes);
      auto materialMapRes = m_frameGraph.AddExternalPermanentResource("MaterialMap", WorldDescriptor(), &m_materialMap);
      auto outputRenderTargetRefRes = m_frameGraph.AddExternalPermanentResource("OutputRef", RenderTargetRefDescriptor(), &m_outputRenderTarget);

      auto geometryPassVS = m_frameGraph.AddExternalPermanentResource("GeometryPassVS", ShaderDescriptor(), m_geometryPassVS);
      auto geometryPassPS = m_frameGraph.AddExternalPermanentResource("GeometryPassPS", ShaderDescriptor(), m_geometryPassPS);

      struct GeometryPassData
      {
         CameraRefResource* TargetCameraRef = nullptr;
         RenderTargetRefResource* OutputRenderTargetRef = nullptr;
         MaterialMapResource* MateralMap = nullptr;
         VertexShaderResource* VertexShader = nullptr;
         PixelShaderResource* PixelShader = nullptr;
         SamplerResource* Sampler = nullptr;
         ConstantBufferResource* TransformBuffer = nullptr;
         ConstantBufferResource* MaterialBuffer = nullptr;
         GBufferResource* OutputGBuffer = nullptr;
         ViewportResource* OutputViewport = nullptr;
         RasterizerStateResource* OutputRasterizerState = nullptr;
      };

      auto geometryPass = m_frameGraph.AddCallbackPass<GeometryPassData>(
         "Geometry Pass",
         [&](Elaina::RenderPassBuilder& builder, GeometryPassData& data)
         {
            data.TargetCameraRef = builder.Read(targetCameraRefRes);
            data.OutputRenderTargetRef = builder.Read(outputRenderTargetRefRes);
            data.MateralMap = builder.Read(materialMapRes);
            data.VertexShader = builder.Read(geometryPassVS);
            data.PixelShader = builder.Read(geometryPassPS);

            SamplerDescriptor samplerDesc;
            samplerDesc.Renderer = this;
            data.Sampler = builder.Create<SamplerResource>("AnisoWrapAlwaysSampler", samplerDesc);

            ConstantBufferDescriptor transformBufferDesc;
            transformBufferDesc.Renderer = this;
            transformBufferDesc.Size = sizeof(GeometryPassTransformBuffer);
            data.TransformBuffer = builder.Create<ConstantBufferResource>("GeometryPassTransform", transformBufferDesc);

            ConstantBufferDescriptor packedMaterialParamsBufferDesc;
            packedMaterialParamsBufferDesc.Renderer = this;
            packedMaterialParamsBufferDesc.Size = sizeof(PackedMaterialParams);
            data.MaterialBuffer = builder.Create<ConstantBufferResource>("GeometryPassPackedMaterialParams", packedMaterialParamsBufferDesc);

            GBufferDescriptor gBufferDesc;
            gBufferDesc.Renderer = this;
            gBufferDesc.OutputRenderTargetReference = &m_outputRenderTarget;
            data.OutputGBuffer = builder.Create<GBufferResource>("GBuffer", gBufferDesc);

            ViewportDescriptor viewportDesc;
            viewportDesc.Renderer = this;
            viewportDesc.OutputRenderTargetReference = &m_outputRenderTarget;
            data.OutputViewport = builder.Create<ViewportResource>("Viewport", viewportDesc);

            RasterizerStateDescriptor rasterizerStateDesc;
            rasterizerStateDesc.Renderer = this;
            data.OutputRasterizerState = builder.Create<RasterizerStateResource>("DefaultRasterizerState", rasterizerStateDesc);
         },
         [&](const GeometryPassData& data)
         {
            /** @todo   Multi-threaded objects rendering */
            ID3D11DeviceContext& deviceContext = GetImmediateContext();
            deviceContext.ClearState();

            deviceContext.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            auto vertexShader = data.VertexShader->GetActual();
            auto pixelShader = data.PixelShader->GetActual();
            auto sampler = data.Sampler->GetActual();
            auto gBuffer = data.OutputGBuffer->GetActual();
            auto transformBuffer = data.TransformBuffer->GetActual();
            auto materialBuffer = data.MaterialBuffer->GetActual();
            auto rasterizerState = data.OutputRasterizerState->GetActual();
            auto viewport = data.OutputViewport->GetActual();
            auto targetCamera = (*data.TargetCameraRef->GetActual());

            vertexShader->Bind(deviceContext);
            pixelShader->Bind(deviceContext);
            sampler->Bind(deviceContext, 0);

            gBuffer->BindAsRenderTarget(deviceContext);
            transformBuffer->Bind(deviceContext, 0, EShaderType::VertexShader);
            materialBuffer->Bind(deviceContext, 0, EShaderType::PixelShader);

            rasterizerState->Bind(deviceContext);
            viewport->Bind(deviceContext);

            auto camTransform = targetCamera->GetTransform();
            Matrix viewMatrix = Matrix::CreateView(
               camTransform->GetPosition(TransformSpace::World),
               camTransform->GetForward(TransformSpace::World),
               camTransform->GetUp(TransformSpace::World));
            Matrix projMatrix = Matrix::CreatePerspectiveProj(
               targetCamera->GetFov(),
               (viewport->GetWidth() / (float)viewport->GetHeight()),
               targetCamera->GetNearPlane(),
               targetCamera->GetFarPlane());

            for (auto batchedMaterial : m_materialMap)
            {
               /* 물리 기반 머테리얼 정보 취득 **/
               Material* material = batchedMaterial.first;
               if (material != nullptr)
               {
                  EMaterialType materialType = material->GetMaterialType();
                  switch (materialType)
                  {
                  case EMaterialType::Opaque:
                  default:
                     Texture2dDX11* baseColorTex = material->GetTexture2D(MaterialTextureProperty::BaseColor)->GetRawTexture();
                     Texture2dDX11* emissiveTex = material->GetTexture2D(MaterialTextureProperty::Emissive)->GetRawTexture();
                     Texture2dDX11* metallicRoughnessTex = material->GetTexture2D(MaterialTextureProperty::MetallicRoughness)->GetRawTexture();
                     Texture2dDX11* specularMapTex = material->GetTexture2D(MaterialTextureProperty::Specular)->GetRawTexture();
                     Texture2dDX11* aoTex = material->GetTexture2D(MaterialTextureProperty::AO)->GetRawTexture();
                     Texture2dDX11* normalTex = material->GetTexture2D(MaterialTextureProperty::Normal)->GetRawTexture();

                     Vector4 baseColorFactor = material->GetVector4Factor(MaterialFactorProperty::BaseColor);
                     Vector4 emissiveFactor = material->GetVector4Factor(MaterialFactorProperty::Emissive);
                     float metallicFactor = material->GetScalarFactor(MaterialFactorProperty::Metallic);
                     float roughnessFactor = material->GetScalarFactor(MaterialFactorProperty::Roughness);
                     float specularFactor = material->GetScalarFactor(MaterialFactorProperty::Specular);
                     Vector2 uvOffset = material->GetVector2Factor(MaterialFactorProperty::UVOffset);


                     auto materialParams = materialBuffer->Map<PackedMaterialParams>(deviceContext);
                     materialParams->BaseColorFactor = baseColorFactor;
                     materialParams->EmissiveColorFactor = emissiveFactor;
                     materialParams->MetallicRoughnessUV = Vector4(metallicFactor, roughnessFactor, uvOffset.x, uvOffset.y);
                     materialParams->SpecularFactor = specularFactor;
                     materialBuffer->UnMap(deviceContext);

                     SAFE_TEX_BIND(baseColorTex, deviceContext, 0, EShaderType::PixelShader);
                     SAFE_TEX_BIND(emissiveTex, deviceContext, 1, EShaderType::PixelShader);
                     SAFE_TEX_BIND(metallicRoughnessTex, deviceContext, 2, EShaderType::PixelShader);
                     SAFE_TEX_BIND(specularMapTex, deviceContext, 3, EShaderType::PixelShader);
                     SAFE_TEX_BIND(aoTex, deviceContext, 4, EShaderType::PixelShader);
                     SAFE_TEX_BIND(normalTex, deviceContext, 5, EShaderType::PixelShader);

                     for (auto meshRenderer : batchedMaterial.second)
                     {
                        Transform* transform = meshRenderer->GetTransform();
                        Mesh* mesh = meshRenderer->GetMesh();
                        if (mesh != nullptr)
                        {
                           Matrix worldMatrix = transform->GetWorldMatrix();
                           Matrix worldViewMatrix = worldMatrix * viewMatrix;
                           auto transforms = transformBuffer->Map<GeometryPassTransformBuffer>(deviceContext);
                           transforms->WorldMatrix = worldMatrix;
                           transforms->WorldViewMatrix = worldViewMatrix;
                           transforms->WorldViewProjMatrix = (worldViewMatrix * projMatrix);
                           transformBuffer->UnMap(deviceContext);
                           mesh->Bind(deviceContext, 0);
                           deviceContext.DrawIndexed(mesh->GetIndexCount(), 0, 0);
                        }
                     }
                  }
               }
            }

            gBuffer->UnbindRenderTarget(deviceContext);
            transformBuffer->Unbind(deviceContext);
            materialBuffer->Unbind(deviceContext);

            vertexShader->Unbind(deviceContext);
            pixelShader->Unbind(deviceContext);
            sampler->Unbind(deviceContext);
         });

      geometryPass->SetCullImmune(true);
      m_frameGraph.Compile();
      m_frameGraph.ExportVisualization("RendererPBR.dot");
      return true;
   }
}