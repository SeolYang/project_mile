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

   DEFINE_CONSTANT_BUFFER(CubemapTransformBuffer)
   {
      Matrix ViewProj;
   };

   DEFINE_CONSTANT_BUFFER(PrefilterParamsBuffer)
   {
      float Roughness;
   };

   DEFINE_CONSTANT_BUFFER(CameraParamsConstantBuffer)
   {
      Vector3 CameraPos;
   };

   DEFINE_CONSTANT_BUFFER(LightParamsConstantBuffer)
   {
      Vector4 LightPos;
      Vector4 LightDirection;
      Vector4 LightRadiance;
      UINT32 LightType;
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
      m_irradianceMap(nullptr),
      m_diffuseIntegralPassVS(nullptr),
      m_diffuseIntegralPassPS(nullptr),
      m_prefilteredEnvMap(nullptr),
      m_prefilterEnvPassVS(nullptr),
      m_prefilterEnvPassPS(nullptr),
      m_brdfLUT(nullptr),
      m_integrateBRDFPassVS(nullptr),
      m_integrateBRDFPassPS(nullptr),
      m_lightingPassVS(nullptr),
      m_lightingPassPS(nullptr)
   {
   }

   RendererPBR::~RendererPBR()
   {
      SafeDelete(m_lightingPassPS);
      SafeDelete(m_lightingPassVS);
      SafeDelete(m_integrateBRDFPassPS);
      SafeDelete(m_integrateBRDFPassVS);
      SafeDelete(m_brdfLUT);
      SafeDelete(m_prefilteredEnvMap);
      SafeDelete(m_irradianceMap);
      SafeDelete(m_diffuseIntegralPassPS);
      SafeDelete(m_diffuseIntegralPassVS);
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

   bool RendererPBR::InitShader()
   {
      /** Geometry Pass Shaders */
      ShaderDescriptor geometryPassDesc;
      geometryPassDesc.Renderer = this;
      geometryPassDesc.FilePath = TEXT("Contents/Shaders/GeometryPass.hlsl");
      m_geometryPassVS = Elaina::Realize<ShaderDescriptor, VertexShaderDX11>(geometryPassDesc);
      if (m_geometryPassVS == nullptr)
      {
         ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to load geometry pass vertex shader!"));
         return false;
      }

      m_geometryPassPS = Elaina::Realize<ShaderDescriptor, PixelShaderDX11>(geometryPassDesc);
      if (m_geometryPassPS == nullptr)
      {
         ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to load geometry pass pixel shader!"));
         return false;
      }

      /** Convert Skybox Pass Shaders */
      ShaderDescriptor convertSkyboxPassDesc;
      convertSkyboxPassDesc.Renderer = this;
      convertSkyboxPassDesc.FilePath = TEXT("Contents/Shaders/Equirectangular2Cube.hlsl");
      m_convertSkyboxPassVS = Elaina::Realize<ShaderDescriptor, VertexShaderDX11>(convertSkyboxPassDesc);
      if (m_convertSkyboxPassVS == nullptr)
      {
         ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to load convert skybox pass vertex shader!"));
         return false;
      }

      m_convertSkyboxPassPS = Elaina::Realize<ShaderDescriptor, PixelShaderDX11>(convertSkyboxPassDesc);
      if (m_convertSkyboxPassPS == nullptr)
      {
         ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to load convert skybox pass vertex shader!"));
         return false;
      }

      /** Diffuse Integral Pass Shaders */
      ShaderDescriptor diffuseIntegralPassDesc;
      diffuseIntegralPassDesc.Renderer = this;
      diffuseIntegralPassDesc.FilePath = TEXT("Contents/Shaders/IrradianceConvolution.hlsl");
      m_diffuseIntegralPassVS = Elaina::Realize<ShaderDescriptor, VertexShaderDX11>(diffuseIntegralPassDesc);
      if (m_diffuseIntegralPassVS == nullptr)
      {
         ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to load diffuse integral pass vertex shader!"));
         return false;
      }

      m_diffuseIntegralPassPS = Elaina::Realize<ShaderDescriptor, PixelShaderDX11>(diffuseIntegralPassDesc);
      if (m_diffuseIntegralPassPS == nullptr)
      {
         ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to load diffuse integral pass pixel shader!"));
         return false;
      }

      /** Prefiltering Environment Map Pass Shaders */
      ShaderDescriptor prefilterEnvPassDesc;
      prefilterEnvPassDesc.Renderer = this;
      prefilterEnvPassDesc.FilePath = TEXT("Contents/Shaders/Prefiltering.hlsl");
      m_prefilterEnvPassVS = Elaina::Realize<ShaderDescriptor, VertexShaderDX11>(prefilterEnvPassDesc);
      if (m_prefilterEnvPassVS == nullptr)
      {
         ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to load prefilter environment map pass vertex shader!"));
         return false;
      }

      m_prefilterEnvPassPS = Elaina::Realize<ShaderDescriptor, PixelShaderDX11>(prefilterEnvPassDesc);
      if (m_prefilterEnvPassPS == nullptr)
      {
         ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to load prefilter environment map pass pixel shader!"));
         return false;
      }

      /** Integrate BRDF Pass Shaders */
      ShaderDescriptor integrateBRDFPassDesc;
      integrateBRDFPassDesc.Renderer = this;
      integrateBRDFPassDesc.FilePath = TEXT("Contents/Shaders/IntegrateBRDF.hlsl");
      m_integrateBRDFPassVS = Elaina::Realize<ShaderDescriptor, VertexShaderDX11>(integrateBRDFPassDesc);
      if (m_integrateBRDFPassVS == nullptr)
      {
         ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to load integrate brdfs pass vertex shader!"));
         return false;
      }

      m_integrateBRDFPassPS = Elaina::Realize<ShaderDescriptor, PixelShaderDX11>(integrateBRDFPassDesc);
      if (m_integrateBRDFPassPS == nullptr)
      {
         ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to load integrate brdfs pass pixel shader!"));
         return false;
      }

      /** Lighting Pass Shaders */
      ShaderDescriptor lightingPassDesc;
      lightingPassDesc.Renderer = this;
      lightingPassDesc.FilePath = TEXT("Contents/Shaders/LightingPass.hlsl");
      m_lightingPassVS = Elaina::Realize<ShaderDescriptor, VertexShaderDX11>(lightingPassDesc);
      if (m_lightingPassVS == nullptr)
      {
         ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to load lighting pass vertex shader!"));
         return false;
      }

      m_lightingPassPS = Elaina::Realize<ShaderDescriptor, PixelShaderDX11>(lightingPassDesc);
      if (m_lightingPassPS == nullptr)
      {
         ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to load lighting pass pixel shader!"));
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

                     SAFE_TEX_UNBIND(baseColorTex, deviceContext);
                     SAFE_TEX_UNBIND(emissiveTex, deviceContext);
                     SAFE_TEX_UNBIND(metallicRoughnessTex, deviceContext);
                     SAFE_TEX_UNBIND(specularMapTex, deviceContext);
                     SAFE_TEX_UNBIND(aoTex, deviceContext);
                     SAFE_TEX_UNBIND(normalTex, deviceContext);
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

      auto geometryPassData = geometryPass->GetData();

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
         BoolRefResource* IBLPrecomputeEnabledRef = nullptr;
         Texture2DRefResource* SkyboxTextureRef = nullptr;
         std::array<MatrixResource*, CUBE_FACES> CaptureViews{ nullptr, };
         ViewportResource* CaptureViewport = nullptr;
         ConstantBufferResource* CaptureTransformBuffer = nullptr;
         RasterizerStateResource* NoCullingState = nullptr;
         DepthStencilStateResource* DepthLessEqualState = nullptr;
         MeshRefResource* CubeMeshRef = nullptr;
         DynamicCubemapRefResource* OutputEnvMapRef = nullptr;
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
         "ConvertSkyboxPassVertexShader",
         ShaderDescriptor(),
         m_convertSkyboxPassVS);

      auto convertSkyboxPassPSRes = m_frameGraph.AddExternalPermanentResource(
         "ConvertSkyboxPassPixelShader",
         ShaderDescriptor(),
         m_convertSkyboxPassPS);

      DynamicCubemapDescriptor envMapDesc;
      envMapDesc.Renderer = this;
      envMapDesc.Size = RendererPBRConstants::ConvertedEnvMapSize;
      m_environmentMap = Elaina::Realize<DynamicCubemapDescriptor, DynamicCubemap>(envMapDesc);

      auto envMapRefRes = m_frameGraph.AddExternalPermanentResource(
         "EnvironmentMapRef",
         DynamicCubemapRefDescriptor(),
         &m_environmentMap);

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

            data.IBLPrecomputeEnabledRef = builder.Create<BoolRefResource>("IBLPrecomputeEnabledRef", BoolRefDescriptor{&m_bPrecomputeIBL});
            data.SkyboxTextureRef = builder.Read(skyboxTextureRefRes);
            for (size_t idx = 0; idx < CUBE_FACES; ++idx)
            {
               data.CaptureViews[idx] = builder.Create<MatrixResource>("CubeCaptureView" + cubeFaces[idx], MatrixDescriptor{ captureViews[idx] });
            }

            ViewportDescriptor viewportDesc;
            viewportDesc.Renderer = this;
            viewportDesc.Width = RendererPBRConstants::ConvertedEnvMapSize;
            viewportDesc.Height = RendererPBRConstants::ConvertedEnvMapSize;
            data.CaptureViewport = builder.Create<ViewportResource>("CaptureViewport", viewportDesc);

            ConstantBufferDescriptor captureTransformDesc;
            captureTransformDesc.Renderer = this;
            captureTransformDesc.Size = sizeof(CubemapTransformBuffer);
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
            data.OutputEnvMapRef = builder.Write(envMapRefRes);
         },
         [](const ConvertSkyboxPassData& data)
         {
            BoolRef bIBLPrecomputeEnabledRef = (*data.IBLPrecomputeEnabledRef->GetActual());
            if (bIBLPrecomputeEnabledRef != nullptr && (*bIBLPrecomputeEnabledRef))
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

               auto outputEnvMap = *(data.OutputEnvMapRef->GetActual());
               auto captureViews = data.CaptureViews;
               for (unsigned int faceIdx = 0; faceIdx < CUBE_FACES; ++faceIdx)
               {
                  outputEnvMap->BindAsRenderTarget(immediateContext, faceIdx);
                  auto mappedTrasnformBuffer = transformBuffer->Map<CubemapTransformBuffer>(immediateContext);
                  (*mappedTrasnformBuffer) = CubemapTransformBuffer{ *captureViews[faceIdx]->GetActual() };
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

      auto convertSkyboxToCubemapPassData = convertSkyboxToCubemapPass->GetData();

      /** Solve Diffuse Integral */
      struct DiffuseIntegralPassData : public RenderPassDataBase
      {
         BoolRefResource* IBLPrecomputeEnabledRef = nullptr;

         SamplerResource* Sampler = nullptr;
         ViewportResource* Viewport = nullptr;
         DepthStencilStateResource* DepthLessEqualState = nullptr;
         RasterizerStateResource* NoCullingState = nullptr;

         std::array<MatrixResource*, CUBE_FACES> CaptureViews{ nullptr, };
         ConstantBufferResource* TransformBuffer = nullptr;
         DynamicCubemapRefResource* EnvironmentMapRef = nullptr;

         MeshRefResource* CubeMeshRef = nullptr;
         DynamicCubemapRefResource* OutputIrradianceMapRef = nullptr;
      };

      auto diffuseIntegralPassVSRes = m_frameGraph.AddExternalPermanentResource(
         "DiffuseIntegralPassVertexShader",
         ShaderDescriptor(),
         m_diffuseIntegralPassVS);

      auto diffuseIntegralPassPSRes = m_frameGraph.AddExternalPermanentResource(
         "DiffuseIntegralPassPixelShader",
         ShaderDescriptor(),
         m_diffuseIntegralPassPS);

      auto irradianceMapRefRes = m_frameGraph.AddExternalPermanentResource(
         "IrradianceMapRef",
         DynamicCubemapRefDescriptor(),
         &m_irradianceMap);

      DynamicCubemapDescriptor irradianceMapDesc;
      irradianceMapDesc.Renderer = this;
      irradianceMapDesc.Size = RendererPBRConstants::IrradianceMapSize;
      m_irradianceMap = Elaina::Realize<DynamicCubemapDescriptor, DynamicCubemap>(irradianceMapDesc);

      auto diffuseIntegralPass = m_frameGraph.AddCallbackPass<DiffuseIntegralPassData>(
         "DiffuseConvolutionPass",
         [&](Elaina::RenderPassBuilder& builder, DiffuseIntegralPassData& data)
         {
            data.Renderer = this;
            data.VertexShader = builder.Read(diffuseIntegralPassVSRes);
            data.PixelShader = builder.Read(diffuseIntegralPassPSRes);

            data.IBLPrecomputeEnabledRef = builder.Read(convertSkyboxToCubemapPassData.IBLPrecomputeEnabledRef);

            data.Sampler = builder.Read(convertSkyboxToCubemapPassData.Sampler);
            data.Viewport = builder.Read(convertSkyboxToCubemapPassData.CaptureViewport);
            data.DepthLessEqualState = builder.Read(convertSkyboxToCubemapPassData.DepthLessEqualState);
            data.NoCullingState = builder.Read(convertSkyboxToCubemapPassData.NoCullingState);

            for (size_t faceIdx = 0; faceIdx < CUBE_FACES; ++faceIdx)
            {
               data.CaptureViews[faceIdx] = builder.Read(convertSkyboxToCubemapPassData.CaptureViews[faceIdx]);
            }

            data.TransformBuffer = builder.Write(convertSkyboxToCubemapPassData.CaptureTransformBuffer);
            data.EnvironmentMapRef = builder.Read(convertSkyboxToCubemapPassData.OutputEnvMapRef);

            data.CubeMeshRef = builder.Read(convertSkyboxToCubemapPassData.CubeMeshRef);
            data.OutputIrradianceMapRef = builder.Write(irradianceMapRefRes);
         },
         [](const DiffuseIntegralPassData& data)
         {
            BoolRef bIBLPrecomputeEnabledRef = (*data.IBLPrecomputeEnabledRef->GetActual());
            if (bIBLPrecomputeEnabledRef != nullptr && (*bIBLPrecomputeEnabledRef))
            {
               ID3D11DeviceContext& immediateContext = data.Renderer->GetImmediateContext();
               immediateContext.ClearState();

               immediateContext.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

               auto vertexShader = data.VertexShader->GetActual();
               auto pixelShader = data.PixelShader->GetActual();
               auto sampler = data.Sampler->GetActual();
               auto viewport = data.Viewport->GetActual();
               auto depthLessEqualState = data.DepthLessEqualState->GetActual();
               auto noCullingState = data.NoCullingState->GetActual();
               auto transformBuffer = data.TransformBuffer->GetActual();
               auto envMap = *data.EnvironmentMapRef->GetActual();
               auto cubeMesh = *data.CubeMeshRef->GetActual();
               auto outputIrradianceMap = *data.OutputIrradianceMapRef->GetActual();

               /** Binds */
               vertexShader->Bind(immediateContext);
               pixelShader->Bind(immediateContext);
               sampler->Bind(immediateContext, 0);
               viewport->Bind(immediateContext);
               depthLessEqualState->Bind(immediateContext);
               noCullingState->Bind(immediateContext);
               transformBuffer->Bind(immediateContext, 0, EShaderType::VertexShader);
               envMap->Bind(immediateContext, 0, EShaderType::PixelShader);
               cubeMesh->Bind(immediateContext, 0);

               /** Render */
               for (unsigned int faceIdx = 0; faceIdx < CUBE_FACES; ++faceIdx)
               {
                  outputIrradianceMap->BindAsRenderTarget(immediateContext, faceIdx);
                  auto mappedTrasnformBuffer = transformBuffer->Map<CubemapTransformBuffer>(immediateContext);
                  (*mappedTrasnformBuffer) = CubemapTransformBuffer{ *data.CaptureViews[faceIdx]->GetActual() };
                  transformBuffer->UnMap(immediateContext);
                  immediateContext.DrawIndexed(cubeMesh->GetIndexCount(), 0, 0);
                  outputIrradianceMap->UnbindAsRenderTarget(immediateContext);
               }
               outputIrradianceMap->GenerateMips(immediateContext);

               /** Unbinds */
               envMap->Unbind(immediateContext);
               transformBuffer->Unbind(immediateContext);
               sampler->Unbind(immediateContext);
               pixelShader->Unbind(immediateContext);
               vertexShader->Unbind(immediateContext);
            }
         });

      auto diffuseIntegralPassData = diffuseIntegralPass->GetData();

      /** ComputePrefilteredEnvMap */
      struct PrefilterEnvPassData : public RenderPassDataBase
      {
         BoolRefResource* IBLPrecomputeEnabledRef = nullptr;

         SamplerResource* Sampler = nullptr;
         std::vector<ViewportResource*> MipViewports;
         DepthStencilStateResource* DepthLessEqualState = nullptr;
         RasterizerStateResource* NoCullingState = nullptr;

         std::array<MatrixResource*, CUBE_FACES> CaptureViews{ nullptr, };
         ConstantBufferResource* TransformBuffer = nullptr;
         ConstantBufferResource* PrefilterParamsBuffer = nullptr;
         DynamicCubemapRefResource* EnvironmentMapRef = nullptr;

         MeshRefResource* CubeMeshRef = nullptr;
         DynamicCubemapRefResource* OutputPrefilteredEnvMapRef = nullptr;
      };

      auto prefilterPassVSRes = m_frameGraph.AddExternalPermanentResource(
         "PrefilterEnvironmentMapPassVertexShader",
         ShaderDescriptor(),
         m_prefilterEnvPassVS);

      auto prefilterPassPSRes = m_frameGraph.AddExternalPermanentResource(
         "PrefilterEnvironmentMapPassPixelShader",
         ShaderDescriptor(),
         m_prefilterEnvPassPS);

      DynamicCubemapDescriptor prefilteredEnvMapDesc;
      prefilteredEnvMapDesc.Renderer = this;
      prefilteredEnvMapDesc.Size = RendererPBRConstants::PrefilteredEnvMapSize;
      m_prefilteredEnvMap = Elaina::Realize<DynamicCubemapDescriptor, DynamicCubemap>(prefilteredEnvMapDesc);

      auto prefilteredEnvMapRefRes = m_frameGraph.AddExternalPermanentResource(
         "PrefilteredEnvironmentMapRef",
         DynamicCubemapRefDescriptor(),
         &m_prefilteredEnvMap);

      auto prefilterEnvMapPass = m_frameGraph.AddCallbackPass<PrefilterEnvPassData>(
         "PrefilterEnvironmentMapPass",
         [&](Elaina::RenderPassBuilder& builder, PrefilterEnvPassData& data)
         {
            data.Renderer = this;
            data.VertexShader = builder.Read(prefilterPassVSRes);
            data.PixelShader = builder.Read(prefilterPassPSRes);

            data.IBLPrecomputeEnabledRef = builder.Read(diffuseIntegralPassData.IBLPrecomputeEnabledRef);

            data.Sampler = builder.Read(diffuseIntegralPassData.Sampler);
            unsigned int maxMipLevels = m_prefilteredEnvMap->GetMaxMipLevels();
            data.MipViewports.resize(maxMipLevels);
            for (unsigned int mipLevel = 0; mipLevel < maxMipLevels; ++mipLevel)
            {
               float viewportSize = std::exp2(static_cast<float>(maxMipLevels - mipLevel));
               ViewportDescriptor viewportDesc;
               viewportDesc.Renderer = this;
               viewportDesc.Width = viewportSize;
               viewportDesc.Height = viewportSize;
               data.MipViewports[mipLevel] = builder.Create<ViewportResource>("Viewport_Mip" + std::to_string(mipLevel), viewportDesc);
            }
            data.DepthLessEqualState = builder.Read(diffuseIntegralPassData.DepthLessEqualState);
            data.NoCullingState = builder.Read(diffuseIntegralPassData.NoCullingState);

            for (size_t faceIdx = 0; faceIdx < CUBE_FACES; ++faceIdx)
            {
               data.CaptureViews[faceIdx] = builder.Read(diffuseIntegralPassData.CaptureViews[faceIdx]);
            }

            data.TransformBuffer = builder.Write(diffuseIntegralPassData.TransformBuffer);
            ConstantBufferDescriptor paramsBufferDesc;
            paramsBufferDesc.Renderer = this;
            paramsBufferDesc.Size = sizeof(PrefilterParamsBuffer);
            data.PrefilterParamsBuffer = builder.Create<ConstantBufferResource>("PrefilterParamsConstantBuffer", paramsBufferDesc);

            data.EnvironmentMapRef = builder.Read(diffuseIntegralPassData.EnvironmentMapRef);

            data.CubeMeshRef = builder.Read(diffuseIntegralPassData.CubeMeshRef);
            data.OutputPrefilteredEnvMapRef = builder.Write(prefilteredEnvMapRefRes);
         },
         [](const PrefilterEnvPassData& data)
         {
            BoolRef bIBLPrecomputeEnabledRef = (*data.IBLPrecomputeEnabledRef->GetActual());
            if (bIBLPrecomputeEnabledRef != nullptr && (*bIBLPrecomputeEnabledRef))
            {
               ID3D11DeviceContext& immediateContext = data.Renderer->GetImmediateContext();
               immediateContext.ClearState();

               immediateContext.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

               auto vertexShader = data.VertexShader->GetActual();
               auto pixelShader = data.PixelShader->GetActual();
               auto sampler = data.Sampler->GetActual();
               auto depthLessEqualState = data.DepthLessEqualState->GetActual();
               auto noCullingState = data.NoCullingState->GetActual();
               auto transformBuffer = data.TransformBuffer->GetActual();
               auto paramsBuffer = data.PrefilterParamsBuffer->GetActual();
               auto envMap = *data.EnvironmentMapRef->GetActual();
               auto cubeMesh = *data.CubeMeshRef->GetActual();
               auto outputPrefilteredEnvMap = *data.OutputPrefilteredEnvMapRef->GetActual();

               /** Binds */
               vertexShader->Bind(immediateContext);
               pixelShader->Bind(immediateContext);
               sampler->Bind(immediateContext, 0);
               depthLessEqualState->Bind(immediateContext);
               noCullingState->Bind(immediateContext);
               transformBuffer->Bind(immediateContext, 0, EShaderType::VertexShader);
               paramsBuffer->Bind(immediateContext, 0, EShaderType::PixelShader);
               envMap->Bind(immediateContext, 0, EShaderType::PixelShader);
               cubeMesh->Bind(immediateContext, 0);

               outputPrefilteredEnvMap->GenerateMips(immediateContext);
               /** Render */
               auto prefilteredEnvMapMaxMips = outputPrefilteredEnvMap->GetMaxMipLevels();
               for (unsigned int mipLevel = 0; mipLevel < prefilteredEnvMapMaxMips; ++mipLevel)
               {
                  float roughness = (mipLevel / static_cast<float>(prefilteredEnvMapMaxMips - 1));
                  auto viewport = data.MipViewports[mipLevel]->GetActual();
                  viewport->Bind(immediateContext);
                  auto mappedParamsBuffer = paramsBuffer->Map<PrefilterParamsBuffer>(immediateContext);
                  (*mappedParamsBuffer) = PrefilterParamsBuffer{roughness};
                  paramsBuffer->UnMap(immediateContext);

                  for (unsigned int faceIdx = 0; faceIdx < CUBE_FACES; ++faceIdx)
                  {
                     auto mappedTrasnformBuffer = transformBuffer->Map<CubemapTransformBuffer>(immediateContext);
                     (*mappedTrasnformBuffer) = CubemapTransformBuffer{ *data.CaptureViews[faceIdx]->GetActual() };
                     transformBuffer->UnMap(immediateContext);

                     outputPrefilteredEnvMap->BindAsRenderTarget(immediateContext, faceIdx, mipLevel);
                     immediateContext.DrawIndexed(cubeMesh->GetIndexCount(), 0, 0);
                     outputPrefilteredEnvMap->UnbindAsRenderTarget(immediateContext);
                  }
               }

               /** Unbinds */
               envMap->Unbind(immediateContext);
               paramsBuffer->Unbind(immediateContext);
               transformBuffer->Unbind(immediateContext);
               sampler->Unbind(immediateContext);
               pixelShader->Unbind(immediateContext);
               vertexShader->Unbind(immediateContext);
            }
         });

      auto prefilterEnvMapPassData = prefilterEnvMapPass->GetData();

      /** Integrate BRDF */
      struct IntegrateBRDFPassData : public RenderPassDataBase
      {
         BoolRefResource* IBLPrecomputeEnabledRef = nullptr;

         SamplerResource* Sampler = nullptr;

         DepthStencilStateResource* DepthDisableState = nullptr;
         ViewportResource* Viewport = nullptr;
         MeshRefResource* QuadMeshRef = nullptr;
         RenderTargetRefResource* OutputBrdfLUTRef = nullptr;
      };

      auto intergrateBRDFPassVSRes = m_frameGraph.AddExternalPermanentResource(
         "IntegrateBRDFsPassVertexShader",
         ShaderDescriptor(),
         m_integrateBRDFPassVS);

      auto integrateBRDFPassPSRes = m_frameGraph.AddExternalPermanentResource(
         "IntegrateBRDFsPassPixelShader",
         ShaderDescriptor(),
         m_integrateBRDFPassPS);

      RenderTargetDescriptor brdfLUTDesc;
      brdfLUTDesc.Renderer = this;
      brdfLUTDesc.Width = RendererPBRConstants::BRDFLUTSize;
      brdfLUTDesc.Height = RendererPBRConstants::BRDFLUTSize;
      brdfLUTDesc.Format = EColorFormat::R16G16_FLOAT;
      m_brdfLUT = Elaina::Realize<RenderTargetDescriptor, RenderTargetDX11>(brdfLUTDesc);

      auto brdfLUTReftRes = m_frameGraph.AddExternalPermanentResource(
         "BrdfLUTRef",
         RenderTargetRefDescriptor(),
         &m_brdfLUT);

      auto quadMeshRefRes = m_frameGraph.AddExternalPermanentResource(
         "QuadMeshRef",
         MeshRefDescriptor(),
         &m_quadMesh);

      auto integrateBRDFPass = m_frameGraph.AddCallbackPass<IntegrateBRDFPassData>(
         "IntegrateBRDFsPass",
         [&](Elaina::RenderPassBuilder& builder, IntegrateBRDFPassData& data)
         {
            data.Renderer = this;
            data.VertexShader = builder.Read(intergrateBRDFPassVSRes);
            data.PixelShader = builder.Read(integrateBRDFPassPSRes);
            data.Sampler = builder.Read(prefilterEnvMapPassData.Sampler);

            data.IBLPrecomputeEnabledRef = builder.Read(prefilterEnvMapPassData.IBLPrecomputeEnabledRef);

            DepthStencilStateDescriptor dsStateDesc;
            dsStateDesc.Renderer = this;
            dsStateDesc.bDepthEnable = false;
            data.DepthDisableState = builder.Create<DepthStencilStateResource>("DepthDisableState", dsStateDesc);

            ViewportDescriptor viewportDesc;
            viewportDesc.Renderer = this;
            viewportDesc.OutputRenderTargetReference = &m_brdfLUT;
            data.Viewport = builder.Create<ViewportResource>("LUTViewport", viewportDesc);
            data.QuadMeshRef = builder.Read(quadMeshRefRes);

            data.OutputBrdfLUTRef = builder.Write(brdfLUTReftRes);
         },
         [](const IntegrateBRDFPassData& data)
         {
            BoolRef bIBLPrecomputeEnabledRef = (*data.IBLPrecomputeEnabledRef->GetActual());
            if (bIBLPrecomputeEnabledRef != nullptr && (*bIBLPrecomputeEnabledRef))
            {
               ID3D11DeviceContext& immediateContext = data.Renderer->GetImmediateContext();
               immediateContext.ClearState();

               immediateContext.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

               auto vertexShader = data.VertexShader->GetActual();
               auto pixelShader = data.PixelShader->GetActual();
               auto sampler = data.Sampler->GetActual();
               auto depthDisableState = data.DepthDisableState->GetActual();
               auto viewport = data.Viewport->GetActual();
               auto quadMesh = *data.QuadMeshRef->GetActual();
               auto outputBrdfLUT = *data.OutputBrdfLUTRef->GetActual();

               viewport->SetWidth(outputBrdfLUT->GetWidth());
               viewport->SetHeight(outputBrdfLUT->GetHeight());

               /** Binds */
               vertexShader->Bind(immediateContext);
               pixelShader->Bind(immediateContext);
               sampler->Bind(immediateContext, 0);
               depthDisableState->Bind(immediateContext);
               viewport->Bind(immediateContext);
               quadMesh->Bind(immediateContext, 0);
               outputBrdfLUT->BindAsRenderTarget(immediateContext);

               /** Render */
               immediateContext.DrawIndexed(quadMesh->GetIndexCount(), 0, 0);

               /** Unbinds */
               outputBrdfLUT->UnbindRenderTarget(immediateContext);
               sampler->Unbind(immediateContext);
               pixelShader->Unbind(immediateContext);
               vertexShader->Unbind(immediateContext);

               (*bIBLPrecomputeEnabledRef) = false;
            }
         });

      auto integrateBRDFPassData = integrateBRDFPass->GetData();

      /** Lighting Pass; Deferred Shading - Lighting */
      struct LightingPassData : public RenderPassDataBase
      {
         SamplerResource* Sampler = nullptr;

         CameraRefResource* CamRef = nullptr;
         LightsDataResource* LightsData = nullptr;
         GBufferResource* GBuffer = nullptr;
         ViewportResource* Viewport = nullptr;
         ConstantBufferResource* CameraParamsBuffer = nullptr;
         ConstantBufferResource* LightParamsBuffer = nullptr;
         DepthStencilStateResource* DepthDisableState = nullptr;
         BlendStateResource* AdditiveBlendState = nullptr;

         MeshRefResource* QuadMeshRef = nullptr;
         RenderTargetResource* OutputHDRBuffer = nullptr;
      };

      auto lightingPassVSRes = m_frameGraph.AddExternalPermanentResource(
         "LightingPassVertexShader",
         ShaderDescriptor(),
         m_lightingPassVS);

      auto lightingPassPSRes = m_frameGraph.AddExternalPermanentResource(
         "LightingPassPixelShader",
         ShaderDescriptor(),
         m_lightingPassPS);

      auto lightsDataRes = m_frameGraph.AddExternalPermanentResource(
         "LightsData",
         WorldDescriptor(),
         &m_lights);

      auto lightingPass = m_frameGraph.AddCallbackPass< LightingPassData>(
         "LightingPass",
         [&](Elaina::RenderPassBuilder& builder, LightingPassData& data)
         {
            data.Renderer = this;
            data.VertexShader = builder.Read(lightingPassVSRes);
            data.PixelShader = builder.Read(lightingPassPSRes);

            data.CamRef = builder.Read(geometryPassData.TargetCameraRef);
            data.LightsData = builder.Read(lightsDataRes);

            SamplerDescriptor samplerDesc;
            samplerDesc.Renderer = this;
            samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
            samplerDesc.AddressModeU = samplerDesc.AddressModeV = samplerDesc.AddressModeW = D3D11_TEXTURE_ADDRESS_BORDER;
            samplerDesc.CompFunc = D3D11_COMPARISON_ALWAYS;
            data.Sampler = builder.Create<SamplerResource>("AnistropicBorderSampler", samplerDesc);

            data.GBuffer = builder.Read(geometryPassData.OutputGBuffer);
            data.Viewport = builder.Read(geometryPassData.OutputViewport);

            ConstantBufferDescriptor cameraParamsDesc;
            cameraParamsDesc.Renderer = this;
            cameraParamsDesc.Size = sizeof(CameraParamsConstantBuffer);
            data.CameraParamsBuffer = builder.Create<ConstantBufferResource>(
               "CameraParamsConstantBuffer",
               cameraParamsDesc);

            ConstantBufferDescriptor lightParamsDesc;
            lightParamsDesc.Renderer = this;
            lightParamsDesc.Size = sizeof(LightParamsConstantBuffer);
            data.LightParamsBuffer = builder.Create<ConstantBufferResource>(
               "LightParamsConstantBuffer",
               lightParamsDesc);

            data.DepthDisableState = builder.Read(integrateBRDFPassData.DepthDisableState);

            BlendStateDescriptor blendStateDesc;
            blendStateDesc.Renderer = this;
            blendStateDesc.BlendDescs[0] = RenderTargetBlendDesc{ 
               true, 
               EBlend::One, EBlend::One, EBlendOP::Add,EBlend::One, EBlend::Zero, EBlendOP::Add,
               (UINT8)EColorWriteEnable::ColorWriteEnableAll };
            data.AdditiveBlendState = builder.Create<BlendStateResource>(
               "AdditiveBlendState",
               blendStateDesc);

            data.QuadMeshRef = builder.Read(integrateBRDFPassData.QuadMeshRef);
            RenderTargetDescriptor outputHDRBufferDesc;
            outputHDRBufferDesc.Renderer = this;
            outputHDRBufferDesc.ResolutionReference = &m_outputRenderTarget;
            outputHDRBufferDesc.Format = EColorFormat::R16G16B16A16_FLOAT;
            data.OutputHDRBuffer = builder.Create<RenderTargetResource>(
               "SourceHDRBuffer",
               outputHDRBufferDesc);
         },
         [](const LightingPassData& data)
         {
            /** @TODO Multi-threaded rendering */
            ID3D11DeviceContext& immediateContext = data.Renderer->GetImmediateContext();

            immediateContext.ClearState();
            immediateContext.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            auto vertexShader = data.VertexShader->GetActual();
            auto pixelShader = data.PixelShader->GetActual();
            auto sampler = data.Sampler->GetActual();
            auto camera = *data.CamRef->GetActual();
            const auto& lightsData = *data.LightsData->GetActual();
            auto depthDisableState = data.DepthDisableState->GetActual();
            auto additiveBlendState = data.AdditiveBlendState->GetActual();
            auto camParamsBuffer = data.CameraParamsBuffer->GetActual();
            auto lightParamsBuffer = data.LightParamsBuffer->GetActual();
            auto gBuffer = data.GBuffer->GetActual();
            auto viewport = data.Viewport->GetActual();
            auto quadMesh = *data.QuadMeshRef->GetActual();
            auto outputHDRBuffer = data.OutputHDRBuffer->GetActual();

            /** Binds */
            vertexShader->Bind(immediateContext);
            pixelShader->Bind(immediateContext);
            sampler->Bind(immediateContext, 0);
            depthDisableState->Bind(immediateContext);
            additiveBlendState->Bind(immediateContext);
            camParamsBuffer->Bind(immediateContext, 0, EShaderType::PixelShader);
            lightParamsBuffer->Bind(immediateContext, 1, EShaderType::PixelShader);
            gBuffer->BindAsShaderResource(immediateContext, 0);
            viewport->Bind(immediateContext);
            quadMesh->Bind(immediateContext, 0);
            outputHDRBuffer->BindAsRenderTarget(immediateContext);

            /** Render */
            Transform* camTransform = camera->GetTransform();
            for (const auto lightComponent : lightsData)
            {
               auto mappedCamParamsBuffer = camParamsBuffer->Map<CameraParamsConstantBuffer>(immediateContext);
               (*mappedCamParamsBuffer) = CameraParamsConstantBuffer{ camTransform->GetPosition(TransformSpace::World) };
               camParamsBuffer->UnMap(immediateContext);

               Transform* lightTransform = lightComponent->GetTransform();
               Vector3 lightPosition = lightTransform->GetPosition(TransformSpace::World);
               Vector3 lightDirection = lightTransform->GetForward(TransformSpace::World);
               Vector3 lightRadiance = lightComponent->GetRadiance();
               auto mappedLightParamsBuffer = lightParamsBuffer->Map<LightParamsConstantBuffer>(immediateContext);
               (*mappedLightParamsBuffer) = LightParamsConstantBuffer
               { 
                  Vector4(lightPosition.x, lightPosition.y, lightPosition.z, 1.0f),
                  Vector4(lightDirection.x, lightDirection.y, lightDirection.z, 0.0f),
                  Vector4(lightRadiance.x, lightRadiance.y, lightRadiance.z, 1.0f),
                  static_cast<UINT32>(lightComponent->GetLightType()) 
               };
               lightParamsBuffer->UnMap(immediateContext);

               immediateContext.DrawIndexed(quadMesh->GetIndexCount(), 0, 0);
            }

            /** Unbinds */
            outputHDRBuffer->UnbindRenderTarget(immediateContext);
            gBuffer->UnbindShaderResource(immediateContext);
            lightParamsBuffer->Unbind(immediateContext);
            camParamsBuffer->Unbind(immediateContext);
            sampler->Unbind(immediateContext);
            pixelShader->Unbind(immediateContext);
            vertexShader->Unbind(immediateContext);
         });

         auto lightingPassData = lightingPass->GetData();
         lightingPass->SetCullImmune(true);

      /** Post-process Pass */

      m_frameGraph.Compile();

      Elaina::VisualizeParams visualizeParams;
      visualizeParams.bSplines = true;
      visualizeParams.RankSep = 2.0;
      m_frameGraph.ExportVisualization("RendererPBR.dot", visualizeParams);
      return true;
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
}