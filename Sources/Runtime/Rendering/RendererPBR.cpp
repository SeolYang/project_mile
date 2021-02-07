#include "Rendering/RendererPBR.h"
#include "Rendering/RenderTargetDX11.h"
#include "Rendering/VertexShaderDX11.h"
#include "Rendering/PixelShaderDX11.h"
#include "Rendering/SamplerDX11.h"
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
#include "Rendering/FrameResourceRealizeImpl.hpp"
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
#include "MT/ThreadPool.h"

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

   DEFINE_CONSTANT_BUFFER(ConvertSkyboxPassTransformBuffer)
   {
      Matrix ViewProj;
   };

   RendererPBR::RendererPBR(Context* context, size_t maximumThreads) :
      RendererDX11(context, maximumThreads),
      m_targetCamera(nullptr),
      m_outputRenderTarget(nullptr),
      m_geometryPassVS(nullptr),
      m_geometryPassPS(nullptr),
      m_convertSkyboxPassVS(nullptr),
      m_convertSkyboxPassPS(nullptr),
      m_skyboxTexture(nullptr),
      m_oldSkyboxTexture(nullptr),
      m_bPrecomputeIBL(true),
      m_quadMesh(nullptr),
      m_cubeMesh(nullptr),
      m_environmentMap(nullptr),
      m_irradianceMap(nullptr)
   {
   }

   RendererPBR::~RendererPBR()
   {
      SafeDelete(m_irradianceMap);
      SafeDelete(m_environmentMap);
      SafeDelete(m_convertSkyboxPassPS);
      SafeDelete(m_convertSkyboxPassVS);
      SafeDelete(m_geometryPassPS);
      SafeDelete(m_geometryPassVS);
      m_frameGraph.Clear();
   }

   bool RendererPBR::Init(Window& window)
   {
      if (RendererDX11::Init(window))
      {
         m_quadMesh = GetPrimitiveQuad();
         m_cubeMesh = GetPrimitiveCube();
         if (InitShader())
         {
            if (InitFrameGraph())
            {
               ME_LOG(MileRendererPBR, Log, TEXT("PBR Renderer Initialized"));
               return true;
            }

            ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to initialize FrameGraph!"));
            return false;
         }

         ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to initialize Shaders!"));
         return false;
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
      auto acquireSkyboxTask = threadPool->AddTask([&]()
         {
            m_skyboxTexture = nullptr;
            auto skyboxComponents = world.GetComponentsFromEntities<SkyboxComponent>();
            if (skyboxComponents.size() > 0)
            {
               m_skyboxTexture = skyboxComponents[0]->GetTexture();
               if (m_skyboxTexture != m_oldSkyboxTexture)
               {
                  m_oldSkyboxTexture = m_skyboxTexture;
                  m_bPrecomputeIBL = true;
               }
            }
         });

      acquireMeshRenderersAndMatTask.get();
      acquireLightsTask.get();
      acquireCamerasTask.get();
      acquireSkyboxTask.get();

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
         return false;
      }

      m_geometryPassPS = new PixelShaderDX11(this);
      if (!m_geometryPassPS->Init(TEXT("Contents/Shaders/GeometryPass.hlsl")))
      {
         ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to load geometry pass pixel shader!"));
         return false;
      }

      /** Convert Skybox Pass Shaders */
      m_convertSkyboxPassVS = new VertexShaderDX11(this);
      if (!m_convertSkyboxPassVS->Init(TEXT("Contents/Shaders/Equirectangular2Cube.hlsl")))
      {
         ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to load convert skybox pass vertex shader!"));
         return false;
      }

      m_convertSkyboxPassPS = new PixelShaderDX11(this);
      if (!m_convertSkyboxPassPS->Init(TEXT("Contents/Shaders/Equirectangular2Cube.hlsl")))
      {
         ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to load convert skybox pass vertex shader!"));
         return false;
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

      /** Geometry Pass */
      auto geometryPassVS = m_frameGraph.AddExternalPermanentResource("GeometryPassVS", ShaderDescriptor(), m_geometryPassVS);
      auto geometryPassPS = m_frameGraph.AddExternalPermanentResource("GeometryPassPS", ShaderDescriptor(), m_geometryPassPS);

      struct GeometryPassData : public RenderPassDataBase
      {
         CameraRefResource* TargetCameraRef = nullptr;
         RenderTargetRefResource* OutputRenderTargetRef = nullptr;
         MaterialMapResource* MateralMap = nullptr;
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
            data.Renderer = this;
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
            ID3D11DeviceContext& deviceContext = data.Renderer->GetImmediateContext();
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

      /** Lighting Pass; IBL */
      static const Matrix captureProj = Matrix::CreatePerspectiveProj(90.0f, 1.0f, 0.1f, 10.0f);
      const std::array<Matrix, CUBE_FACES> captureViews =
      {
         // +x
         Matrix::CreateView(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f)) * captureProj,
         // -x
         Matrix::CreateView(Vector3(0.0f, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f)) * captureProj,
         // +y
         Matrix::CreateView(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f)) * captureProj,
         // -y
         Matrix::CreateView(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f)) * captureProj,
         // +z
         Matrix::CreateView(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f)) * captureProj,
         // -z
         Matrix::CreateView(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 1.0f, 0.0f)) * captureProj
      };
      const std::array<std::string, CUBE_FACES> cubeFaces =
      {
         "+x",
         "-x",
         "+y",
         "-y",
         "+z",
         "-z"
      };

      /** Calculate Diffuse Irradiance */

      /** Convert Equirect To Cubemap */
      struct ConvertSkyboxPassData : public RenderPassDataBase
      {
         SamplerResource* Sampler = nullptr;
         BoolRefResource* IBLPrecomputeEnabled = nullptr;
         Texture2DRefResource* SkyboxTextureRef = nullptr;
         std::array<MatrixResource*, CUBE_FACES> CaptureViews{ nullptr, };
         ViewportResource* CaptureViewport = nullptr;
         ConstantBufferResource* CaptureTransformBuffer = nullptr;
         RasterizerStateResource* NoCullingState = nullptr;
         DepthStencilStateResource* DepthLessEqualState = nullptr;
         MeshRefResource* CubeMeshRef = nullptr;
         DynamicCubemapResource* OutputEnvMap = nullptr;
      };

      auto skyboxTextureRefRes = m_frameGraph.AddExternalPermanentResource(
         "SkyboxTextureRef",
         Texture2DRefDescriptor(),
         &m_skyboxTexture);

      auto cubeMeshRefRes = m_frameGraph.AddExternalPermanentResource(
         "CubeMeshRef",
         MeshRefDescriptor(),
         &m_cubeMesh);

      auto convertSkyboxPassVSRes = m_frameGraph.AddExternalPermanentResource(
         "ConvertSkyboxPassVS",
         ShaderDescriptor(),
         m_convertSkyboxPassVS);

      auto convertSkyboxPassPSRes = m_frameGraph.AddExternalPermanentResource(
         "ConvertSkyboxPassPS",
         ShaderDescriptor(),
         m_convertSkyboxPassPS);

      DynamicCubemapDescriptor envMapDesc;
      envMapDesc.Renderer = this;
      envMapDesc.Size = RendererPBRConstants::ConvertedEnvMapSize;
      m_environmentMap = Elaina::Realize<DynamicCubemapDescriptor, DynamicCubemap>(envMapDesc);

      auto environmentMapResource = m_frameGraph.AddExternalPermanentResource(
         "EnvironmentMap",
         envMapDesc,
         m_environmentMap);

      auto convertSkyboxToCubemapPass = m_frameGraph.AddCallbackPass<ConvertSkyboxPassData>(
         "ConvertTextureToCubemap",
         [&](Elaina::RenderPassBuilder& builder, ConvertSkyboxPassData& data)
         {
            data.Renderer = this;
            data.VertexShader = builder.Read(convertSkyboxPassVSRes);
            data.PixelShader = builder.Read(convertSkyboxPassPSRes);

            SamplerDescriptor samplerDesc;
            samplerDesc.Renderer = this;
            samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            samplerDesc.AddressModeU = samplerDesc.AddressModeV = samplerDesc.AddressModeW = D3D11_TEXTURE_ADDRESS_CLAMP;
            samplerDesc.CompFunc = D3D11_COMPARISON_ALWAYS;
            data.Sampler = builder.Create<SamplerResource>("LinearClampAlwaysSampler", samplerDesc);

            data.IBLPrecomputeEnabled = builder.Create<BoolRefResource>("IBLPrecomputeEnabled", BoolRefDescriptor{&m_bPrecomputeIBL});
            data.SkyboxTextureRef = builder.Read(skyboxTextureRefRes);
            for (size_t idx = 0; idx < CUBE_FACES; ++idx)
            {
               data.CaptureViews[idx] = builder.Create<MatrixResource>("IBLCaptureView" + cubeFaces[idx], MatrixDescriptor{ captureViews[idx] });
            }

            ViewportDescriptor viewportDesc;
            viewportDesc.Renderer = this;
            viewportDesc.Width = RendererPBRConstants::ConvertedEnvMapSize;
            viewportDesc.Height = RendererPBRConstants::ConvertedEnvMapSize;
            data.CaptureViewport = builder.Create<ViewportResource>("CaptureViewport", viewportDesc);

            ConstantBufferDescriptor captureTransformDesc;
            captureTransformDesc.Renderer = this;
            captureTransformDesc.Size = sizeof(ConvertSkyboxPassTransformBuffer);
            data.CaptureTransformBuffer = builder.Create<ConstantBufferResource>("CaptureTransformBuffer", captureTransformDesc);

            RasterizerStateDescriptor noCullingDesc;
            noCullingDesc.Renderer = this;
            noCullingDesc.CullMode = ECullMode::None;
            data.NoCullingState = builder.Create<RasterizerStateResource>("NoCullingState", noCullingDesc);

            DepthStencilStateDescriptor depthLessEqualDesc;
            depthLessEqualDesc.Renderer = this;
            depthLessEqualDesc.bDepthEnable = true;
            depthLessEqualDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
            data.DepthLessEqualState = builder.Create<DepthStencilStateResource>("DepthLessEqualState", depthLessEqualDesc);

            data.CubeMeshRef = builder.Read(cubeMeshRefRes);
            data.OutputEnvMap = builder.Write(environmentMapResource);
         },
         [](const ConvertSkyboxPassData& data)
         {
            BoolRef bIBLPrecomputeEnabled = (*data.IBLPrecomputeEnabled->GetActual());
            if (bIBLPrecomputeEnabled != nullptr && (*bIBLPrecomputeEnabled))
            {
               ID3D11DeviceContext& immediateContext = data.Renderer->GetImmediateContext();
               immediateContext.ClearState();

               immediateContext.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

               auto vertexShader = data.VertexShader->GetActual();
               auto pixelShader = data.PixelShader->GetActual();
               auto sampler = data.Sampler->GetActual();
               auto viewport = data.CaptureViewport->GetActual();
               auto skyboxTexture = (*data.SkyboxTextureRef->GetActual());
               auto transformBuffer = data.CaptureTransformBuffer->GetActual();

               /** Binds */
               vertexShader->Bind(immediateContext);
               transformBuffer->Bind(immediateContext, 0, EShaderType::VertexShader);
               pixelShader->Bind(immediateContext);
               sampler->Bind(immediateContext, 0);
               viewport->Bind(immediateContext);
               Texture2dDX11* skyboxTextureDX11 = nullptr;
               if (skyboxTexture != nullptr)
               {
                  skyboxTextureDX11 = skyboxTexture->GetRawTexture();
                  if (skyboxTextureDX11 != nullptr)
                  {
                     skyboxTextureDX11->Bind(immediateContext, 0, EShaderType::PixelShader);
                  }
               }

               auto depthLessEqualState = data.DepthLessEqualState->GetActual();
               depthLessEqualState->Bind(immediateContext);
               auto noCullingState = data.NoCullingState->GetActual();
               noCullingState->Bind(immediateContext);
               auto cubeMesh = *data.CubeMeshRef->GetActual();
               cubeMesh->Bind(immediateContext, 0);

               auto outputEnvMap = data.OutputEnvMap->GetActual();
               auto captureViews = data.CaptureViews;
               for (unsigned int faceIdx = 0; faceIdx < CUBE_FACES; ++faceIdx)
               {
                  outputEnvMap->BindAsRenderTarget(immediateContext, faceIdx);
                  auto mappedTrasnformBuffer = transformBuffer->Map<ConvertSkyboxPassTransformBuffer>(immediateContext);
                  (*mappedTrasnformBuffer) = ConvertSkyboxPassTransformBuffer{ *captureViews[faceIdx]->GetActual() };
                  transformBuffer->UnMap(immediateContext);
                  immediateContext.DrawIndexed(cubeMesh->GetIndexCount(), 0, 0);
                  outputEnvMap->UnbindAsRenderTarget(immediateContext);
               }
               outputEnvMap->GenerateMips(immediateContext);

               /** Unbinds */
               if (skyboxTextureDX11 != nullptr)
               {
                  skyboxTextureDX11->Unbind(immediateContext);
               }
               sampler->Unbind(immediateContext);
               pixelShader->Unbind(immediateContext);
               transformBuffer->Unbind(immediateContext);
               vertexShader->Unbind(immediateContext);
            }
         });

      convertSkyboxToCubemapPass->SetCullImmune(true);

      /** Solve Diffuse Integral */
      struct DiffuseIntegralPassData : public RenderPassDataBase
      {

      };

      /** ComputePrefilteredEnvMap */

      /** Integrate BRDF */

      /** Lighting Pass; Deferred Shading - Lighting */

      /** Post-process Pass */

      m_frameGraph.Compile();

      Elaina::VisualizeParams visualizeParams;
      visualizeParams.bSplines = true;
      m_frameGraph.ExportVisualization("RendererPBR.dot", visualizeParams);
      return true;
   }
}