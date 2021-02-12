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
#include <random>
#include <iterator>

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

   DEFINE_CONSTANT_BUFFER(OneMatrixConstantBuffer)
   {
      Matrix Mat;
   };

   DEFINE_CONSTANT_BUFFER(SSAOParamsConstantBuffer)
   {
      Vector4 Samples[64];
      Vector2 NoiseScale;
      Matrix Projection;
      float Radius;
      float Bias;
      float Magnitude;
   };

   DEFINE_CONSTANT_BUFFER(AmbientParamsConstantBuffer)
   {
      Vector3 CameraPos;
      float Ao;
      unsigned int SSAOEnabled;
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
      m_lightingPassPS(nullptr),
      m_gBufferToViewSpacePassVS(nullptr),
      m_gBufferToViewSpacePassPS(nullptr),
      m_ssaoPassVS(nullptr),
      m_ssaoPassPS(nullptr),
      m_ssaoBlurPassVS(nullptr),
      m_ssaoBlurPassPS(nullptr),
      m_bSSAOEnabled(true),
      m_ambientEmissivePassVS(nullptr),
      m_ambientEmissivePassPS(nullptr),
      m_globalAOFactor(1.0f),
      m_skyboxPassVS(nullptr),
      m_skyboxPassPS(nullptr),
      m_renderSkyboxType(ESkyboxType::EnvironmentMap)
   {
   }

   RendererPBR::~RendererPBR()
   {
      SafeDelete(m_skyboxPassPS);
      SafeDelete(m_skyboxPassVS);
      SafeDelete(m_ambientEmissivePassPS);
      SafeDelete(m_ambientEmissivePassVS);
      SafeDelete(m_ssaoBlurPassPS);
      SafeDelete(m_ssaoBlurPassVS);
      SafeDelete(m_ssaoPassPS);
      SafeDelete(m_ssaoPassVS);
      SafeDelete(m_gBufferToViewSpacePassPS);
      SafeDelete(m_gBufferToViewSpacePassVS);
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

      /** GBuffer Convert Pass Shader(to view space) */
      ShaderDescriptor gBufferConvertPassDesc;
      gBufferConvertPassDesc.Renderer = this;
      gBufferConvertPassDesc.FilePath = TEXT("Contents/Shaders/ViewSpaceGBuffer.hlsl");
      m_gBufferToViewSpacePassVS = Elaina::Realize<ShaderDescriptor, VertexShaderDX11>(gBufferConvertPassDesc);
      if (m_gBufferToViewSpacePassVS == nullptr)
      {
         ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to load GBuffer convert pass vertex shader!"));
         return false;
      }

      m_gBufferToViewSpacePassPS = Elaina::Realize<ShaderDescriptor, PixelShaderDX11>(gBufferConvertPassDesc);
      if (m_gBufferToViewSpacePassPS == nullptr)
      {
         ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to load GBuffer convert pass pixel shader!"));
         return false;
      }

      /** SSAO Pass Shaders  */
      ShaderDescriptor ssaoPassDesc;
      ssaoPassDesc.Renderer = this;
      ssaoPassDesc.FilePath = TEXT("Contents/Shaders/SSAO.hlsl");
      m_ssaoPassVS = Elaina::Realize<ShaderDescriptor, VertexShaderDX11>(ssaoPassDesc);
      if (m_ssaoPassVS == nullptr)
      {
         ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to load ssao pass vertex shader!"));
         return false;
      }

      m_ssaoPassPS = Elaina::Realize<ShaderDescriptor, PixelShaderDX11>(ssaoPassDesc);
      if (m_ssaoPassPS == nullptr)
      {
         ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to load ssao pass pixel shader!"));
         return false;
      }

      /** SSAO Blur Pass Shaders  */
      ShaderDescriptor ssaoBlurPassDesc;
      ssaoBlurPassDesc.Renderer = this;
      ssaoBlurPassDesc.FilePath = TEXT("Contents/Shaders/SSAOBlur.hlsl");
      m_ssaoBlurPassVS = Elaina::Realize<ShaderDescriptor, VertexShaderDX11>(ssaoBlurPassDesc);
      if (m_ssaoBlurPassVS == nullptr)
      {
         ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to load ssao blur pass vertex shader!"));
         return false;
      }

      m_ssaoBlurPassPS = Elaina::Realize<ShaderDescriptor, PixelShaderDX11>(ssaoBlurPassDesc);
      if (m_ssaoBlurPassPS == nullptr)
      {
         ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to load ssao blur pass pixel shader!"));
         return false;
      }

      /** Ambient Emissive Pass Shaders  */
      ShaderDescriptor ambientEmissivePassDesc;
      ambientEmissivePassDesc.Renderer = this;
      ambientEmissivePassDesc.FilePath = TEXT("Contents/Shaders/AmbientEmissivePass.hlsl");
      m_ambientEmissivePassVS = Elaina::Realize<ShaderDescriptor, VertexShaderDX11>(ambientEmissivePassDesc);
      if (m_ambientEmissivePassVS == nullptr)
      {
         ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to load ambient emissive pass vertex shader!"));
         return false;
      }

      m_ambientEmissivePassPS = Elaina::Realize<ShaderDescriptor, PixelShaderDX11>(ambientEmissivePassDesc);
      if (m_ambientEmissivePassPS == nullptr)
      {
         ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to load ambient emissive pass pixel shader!"));
         return false;
      }

      /** Skybox Pass Shaders  */
      ShaderDescriptor skyboxPassDesc;
      skyboxPassDesc.Renderer = this;
      skyboxPassDesc.FilePath = TEXT("Contents/Shaders/SkyboxPass.hlsl");
      m_skyboxPassVS = Elaina::Realize<ShaderDescriptor, VertexShaderDX11>(skyboxPassDesc);
      if (m_ambientEmissivePassVS == nullptr)
      {
         ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to load skybox pass vertex shader!"));
         return false;
      }

      m_skyboxPassPS = Elaina::Realize<ShaderDescriptor, PixelShaderDX11>(skyboxPassDesc);
      if (m_ambientEmissivePassPS == nullptr)
      {
         ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to load skybox pass pixel shader!"));
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
            captureTransformDesc.Size = sizeof(OneMatrixConstantBuffer);
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
                  auto mappedTrasnformBuffer = transformBuffer->Map<OneMatrixConstantBuffer>(immediateContext);
                  (*mappedTrasnformBuffer) = OneMatrixConstantBuffer{ *captureViews[faceIdx]->GetActual() };
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
                  auto mappedTrasnformBuffer = transformBuffer->Map<OneMatrixConstantBuffer>(immediateContext);
                  (*mappedTrasnformBuffer) = OneMatrixConstantBuffer{ *data.CaptureViews[faceIdx]->GetActual() };
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
                     auto mappedTrasnformBuffer = transformBuffer->Map<OneMatrixConstantBuffer>(immediateContext);
                     (*mappedTrasnformBuffer) = OneMatrixConstantBuffer{ *data.CaptureViews[faceIdx]->GetActual() };
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
      struct ConvertGBufferPassData : public RenderPassDataBase
      {
         SamplerResource* Sampler = nullptr;
         CameraRefResource* CamRef = nullptr;
         ConstantBufferResource* ConvertParamsBuffer = nullptr;

         ViewportResource* Viewport = nullptr;
         MeshRefResource* QuadMeshRef = nullptr;
         DepthStencilStateResource* DepthDisableState = nullptr;

         GBufferResource* SourceGBuffer = nullptr;
         GBufferResource* ConvertedGBuffer = nullptr;
      };

      auto gBufferConvertPassVSRes = m_frameGraph.AddExternalPermanentResource(
         "ConvertGBufferToViewSpaceVertexShader",
         ShaderDescriptor(),
         m_gBufferToViewSpacePassVS);

      auto gBufferConvertPassPSRes = m_frameGraph.AddExternalPermanentResource(
         "ConvertGBufferToViewSpacePixelShader",
         ShaderDescriptor(),
         m_gBufferToViewSpacePassPS);

      auto gBufferConvertPass = m_frameGraph.AddCallbackPass<ConvertGBufferPassData>(
         "ConvertGBufferToViewSpacePass",
         [&](Elaina::RenderPassBuilder& builder, ConvertGBufferPassData& data)
         {
            data.Renderer = this;
            data.VertexShader = builder.Read(gBufferConvertPassVSRes);
            data.PixelShader = builder.Read(gBufferConvertPassPSRes);

            SamplerDescriptor samplerDesc;
            samplerDesc.Renderer = this;
            samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
            samplerDesc.AddressModeU = samplerDesc.AddressModeV = samplerDesc.AddressModeW = D3D11_TEXTURE_ADDRESS_CLAMP;
            samplerDesc.CompFunc = D3D11_COMPARISON_ALWAYS;
            data.Sampler = builder.Create<SamplerResource>(
               "PointClampSampler",
               samplerDesc);

            data.CamRef = builder.Read(lightingPassData.CamRef);

            ConstantBufferDescriptor paramsDesc;
            paramsDesc.Renderer = this;
            paramsDesc.Size = sizeof(OneMatrixConstantBuffer);
            data.ConvertParamsBuffer = builder.Create<ConstantBufferResource>(
               "ConvertParamsConstantBuffer",
               paramsDesc);

            data.Viewport = builder.Read(geometryPassData.OutputViewport);
            data.QuadMeshRef = builder.Read(integrateBRDFPassData.QuadMeshRef);
            data.DepthDisableState = builder.Read(integrateBRDFPassData.DepthDisableState);

            data.SourceGBuffer = builder.Read(geometryPassData.OutputGBuffer);

            GBufferDescriptor convertedGBufferDesc;
            convertedGBufferDesc.Renderer = this;
            convertedGBufferDesc.OutputRenderTargetReference = &m_outputRenderTarget;
            data.ConvertedGBuffer = builder.Create<GBufferResource>("ViewSpaceGBuffer", convertedGBufferDesc);
         },
         [](const ConvertGBufferPassData& data)
         {
            ID3D11DeviceContext& immediateContext = data.Renderer->GetImmediateContext();
            
            immediateContext.ClearState();
            immediateContext.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            auto vertexShader = data.VertexShader->GetActual();
            auto pixelShader = data.PixelShader->GetActual();
            auto sampler = data.Sampler->GetActual();
            auto convertParamsBuffer = data.ConvertParamsBuffer->GetActual();
            auto sourceGBuffer = data.SourceGBuffer->GetActual();
            auto convertedGBuffer = data.ConvertedGBuffer->GetActual();
            auto camera = *data.CamRef->GetActual();
            auto viewport = data.Viewport->GetActual();
            auto quadMesh = *data.QuadMeshRef->GetActual();
            auto depthDisableState = data.DepthDisableState->GetActual();

            /** Binds */
            vertexShader->Bind(immediateContext);
            pixelShader->Bind(immediateContext);
            sampler->Bind(immediateContext, 0);
            convertParamsBuffer->Bind(immediateContext, 0, EShaderType::PixelShader);
            viewport->Bind(immediateContext);
            quadMesh->Bind(immediateContext, 0);
            depthDisableState->Bind(immediateContext);
            sourceGBuffer->BindAsShaderResource(immediateContext, 0, true);
            convertedGBuffer->BindAsRenderTarget(immediateContext);

            /** Render */
            auto camTransform = camera->GetTransform();
            Matrix viewMatrix = Matrix::CreateView(
               camTransform->GetPosition(TransformSpace::World),
               camTransform->GetForward(TransformSpace::World),
               camTransform->GetUp(TransformSpace::World));

            auto mappedConvertParams = convertParamsBuffer->Map<OneMatrixConstantBuffer>(immediateContext);
            (*mappedConvertParams) = OneMatrixConstantBuffer{ viewMatrix };
            convertParamsBuffer->UnMap(immediateContext);
            immediateContext.DrawIndexed(quadMesh->GetIndexCount(), 0, 0);

            /** Unbinds */
            convertedGBuffer->UnbindRenderTarget(immediateContext);
            sourceGBuffer->UnbindShaderResource(immediateContext);
            convertParamsBuffer->Unbind(immediateContext);
            sampler->Unbind(immediateContext);
            pixelShader->Unbind(immediateContext);
            vertexShader->Unbind(immediateContext);
         });

      auto convertGBufferPassData = gBufferConvertPass->GetData();

      /** SSAO */
      struct SSAOPassData : public RenderPassDataBase
      {
         BoolRefResource* SSAOEnabledRef = nullptr;
         SamplerResource* Sampler = nullptr;
         SamplerResource* NoiseSampler = nullptr;
         SamplerResource* PositionSampler = nullptr;

         ViewportResource* Viewport = nullptr;
         DepthStencilStateResource* DepthDisableState = nullptr;

         CameraRefResource* CamRef = nullptr;

         ConstantBufferResource* SSAOParamsBuffer = nullptr;

         Texture2dDX11Resource* NoiseTexture = nullptr;
         GBufferResource* ViewspaceGBuffer = nullptr;

         MeshRefResource* QuadMeshRef = nullptr;
         RenderTargetResource* Output = nullptr;
      };

      auto ssaoPassVSRes = m_frameGraph.AddExternalPermanentResource(
         "SSAOPassVertexShader",
         ShaderDescriptor(),
         m_ssaoPassVS);

      auto ssaoPassPSRes = m_frameGraph.AddExternalPermanentResource(
         "SSAOPassPixelShader",
         ShaderDescriptor(),
         m_ssaoPassPS);

      auto ssaoPass = m_frameGraph.AddCallbackPass<SSAOPassData>(
         "SSAOPass",
         [&](Elaina::RenderPassBuilder& builder, SSAOPassData& data)
         {
            data.Renderer = this;
            data.VertexShader = builder.Read(ssaoPassVSRes);
            data.PixelShader = builder.Read(ssaoPassPSRes);

            BoolRefDescriptor ssaoEnabledDesc;
            ssaoEnabledDesc.Reference = &m_bSSAOEnabled;
            data.SSAOEnabledRef = builder.Create<BoolRefResource>("SSAOEnabledRef", ssaoEnabledDesc);

            SamplerDescriptor samplerDesc;
            samplerDesc.Renderer = this;
            samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
            samplerDesc.AddressModeU = samplerDesc.AddressModeV = samplerDesc.AddressModeW = D3D11_TEXTURE_ADDRESS_CLAMP;
            samplerDesc.CompFunc = D3D11_COMPARISON_ALWAYS;
            data.Sampler = builder.Create<SamplerResource>("SSAOSampler", samplerDesc);

            SamplerDescriptor noiseSamplerDesc;
            noiseSamplerDesc.Renderer = this;
            noiseSamplerDesc.Filter = samplerDesc.Filter;
            noiseSamplerDesc.AddressModeU = noiseSamplerDesc.AddressModeV = noiseSamplerDesc.AddressModeW = D3D11_TEXTURE_ADDRESS_WRAP;
            noiseSamplerDesc.CompFunc = D3D11_COMPARISON_ALWAYS;
            data.NoiseSampler = builder.Create<SamplerResource>("NoiseSampler", noiseSamplerDesc);

            data.PositionSampler = builder.Create<SamplerResource>("PositionSampler", samplerDesc);

            data.Viewport = builder.Read(lightingPassData.Viewport);
            data.DepthDisableState = builder.Read(lightingPassData.DepthDisableState);

            data.CamRef = builder.Read(lightingPassData.CamRef);

            ConstantBufferDescriptor ssaoParamsBufferDesc;
            ssaoParamsBufferDesc.Renderer = this;
            ssaoParamsBufferDesc.Size = sizeof(SSAOParamsConstantBuffer);
            data.SSAOParamsBuffer = builder.Create<ConstantBufferResource>(
               "SSAOParamsConstantBuffer",
               ssaoParamsBufferDesc);

            Texture2dDX11Descriptor noiseTexDesc;
            noiseTexDesc.Renderer = this;
            noiseTexDesc.Width = noiseTexDesc.Height = RendererPBRConstants::SSAONoiseTextureSize;
            noiseTexDesc.Channels = 4;
            noiseTexDesc.Format = static_cast<DXGI_FORMAT>(EColorFormat::R32G32B32A32_FLOAT);
            noiseTexDesc.Data = (unsigned char*)(m_ssaoParams.Noise.data());
            data.NoiseTexture = builder.Create<Texture2dDX11Resource>(
               "SSAONoiseTexture",
               noiseTexDesc);

            data.ViewspaceGBuffer = builder.Read(convertGBufferPassData.ConvertedGBuffer);

            data.QuadMeshRef = builder.Read(lightingPassData.QuadMeshRef);

            RenderTargetDescriptor outputSSAODesc;
            outputSSAODesc.Renderer = this;
            outputSSAODesc.ResolutionReference = &m_outputRenderTarget;
            outputSSAODesc.Format = EColorFormat::R32_FLOAT;
            data.Output = builder.Create<RenderTargetResource>(
               "SSAOOutput",
               outputSSAODesc);
         },
         [](const SSAOPassData& data)
         {
            bool bSSAOEnabled = *(*data.SSAOEnabledRef->GetActual());
            if (bSSAOEnabled)
            {
               ID3D11DeviceContext& immeidiateContext = data.Renderer->GetImmediateContext();

               immeidiateContext.ClearState();
               immeidiateContext.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

               auto vertexShader = data.VertexShader->GetActual();
               auto pixelShader = data.PixelShader->GetActual();
               auto sampler = data.Sampler->GetActual();
               auto noiseSampler = data.NoiseSampler->GetActual();
               auto posSampler = data.PositionSampler->GetActual();
               auto viewport = data.Viewport->GetActual();
               auto depthDisableState = data.DepthDisableState->GetActual();
               auto camera = *data.CamRef->GetActual();
               auto ssaoParamsBuffer = data.SSAOParamsBuffer->GetActual();
               auto noiseTexture = data.NoiseTexture->GetActual();
               auto viewspaceGBuffer = data.ViewspaceGBuffer->GetActual();
               auto quadMesh = *data.QuadMeshRef->GetActual();
               auto output = data.Output->GetActual();

               /** Binds */
               vertexShader->Bind(immeidiateContext);
               pixelShader->Bind(immeidiateContext);
               sampler->Bind(immeidiateContext, 0);
               noiseSampler->Bind(immeidiateContext, 1);
               posSampler->Bind(immeidiateContext, 2);
               viewport->Bind(immeidiateContext);
               depthDisableState->Bind(immeidiateContext);
               ssaoParamsBuffer->Bind(immeidiateContext, 0, EShaderType::PixelShader);
               noiseTexture->Bind(immeidiateContext, 5, EShaderType::PixelShader);
               viewspaceGBuffer->BindAsShaderResource(immeidiateContext, 0);
               quadMesh->Bind(immeidiateContext, 0);
               output->BindAsRenderTarget(immeidiateContext);

               /** Render */
               auto camTransform = camera->GetTransform();
               Matrix projMatrix = Matrix::CreatePerspectiveProj(
                  camera->GetFov(),
                  (output->GetWidth() / (float)output->GetHeight()),
                  camera->GetNearPlane(),
                  camera->GetFarPlane());

               auto ssaoParams = ((RendererPBR*)data.Renderer)->GetSSAOParams();

               auto mappedSSAOParamsBuffer = ssaoParamsBuffer->Map<SSAOParamsConstantBuffer>(immeidiateContext);
               ZeroMemory(mappedSSAOParamsBuffer, sizeof(SSAOParamsConstantBuffer));
               std::copy(std::begin(ssaoParams.Samples), std::end(ssaoParams.Samples), std::begin((*mappedSSAOParamsBuffer).Samples));
               (*mappedSSAOParamsBuffer).NoiseScale = ssaoParams.NoiseScale;
               (*mappedSSAOParamsBuffer).Projection = projMatrix;
               (*mappedSSAOParamsBuffer).Radius = ssaoParams.Radius;
               (*mappedSSAOParamsBuffer).Bias = ssaoParams.Bias;
               (*mappedSSAOParamsBuffer).Magnitude = ssaoParams.Magnitude;
               ssaoParamsBuffer->UnMap(immeidiateContext);
               immeidiateContext.DrawIndexed(quadMesh->GetIndexCount(), 0, 0);

               /** Unbinds */
               output->UnbindRenderTarget(immeidiateContext);
               viewspaceGBuffer->UnbindShaderResource(immeidiateContext);
               noiseTexture->Unbind(immeidiateContext);
               ssaoParamsBuffer->Unbind(immeidiateContext);
               posSampler->Unbind(immeidiateContext);
               noiseSampler->Unbind(immeidiateContext);
               sampler->Unbind(immeidiateContext);
               pixelShader->Unbind(immeidiateContext);
               vertexShader->Unbind(immeidiateContext);
            }
         });

      auto ssaoPassData = ssaoPass->GetData();

      /** SSAO Blur */
      struct SSAOBlurPassData : public RenderPassDataBase
      {
         BoolRefResource* SSAOEnabledRef = nullptr;

         SamplerResource* Sampler = nullptr;

         RenderTargetResource* Source = nullptr;
         ViewportResource* Viewport = nullptr;
         DepthStencilStateResource* DepthDisableState = nullptr;

         MeshRefResource* QuadMeshRef = nullptr;
         RenderTargetResource* Output = nullptr;
      };

      auto ssaoBlurPassVSRes = m_frameGraph.AddExternalPermanentResource("SSAOBlurPassVertexShader", ShaderDescriptor(), m_ssaoBlurPassVS);
      auto ssaoBlurPassPSRes = m_frameGraph.AddExternalPermanentResource("SSAOBlurPassPixelShader", ShaderDescriptor(), m_ssaoBlurPassPS);

      auto ssaoBlurPass = m_frameGraph.AddCallbackPass<SSAOBlurPassData>(
         "SSAOBlurPass",
         [&](Elaina::RenderPassBuilder& builder, SSAOBlurPassData& data)
         {
            data.Renderer = this;
            data.VertexShader = builder.Read(ssaoBlurPassVSRes);
            data.PixelShader = builder.Read(ssaoBlurPassPSRes);

            data.SSAOEnabledRef = builder.Read(ssaoPassData.SSAOEnabledRef);

            data.Sampler = builder.Read(ssaoPassData.Sampler);

            data.Source = builder.Read(ssaoPassData.Output);
            data.Viewport = builder.Read(ssaoPassData.Viewport);
            data.DepthDisableState = builder.Read(ssaoPassData.DepthDisableState);

            data.QuadMeshRef = builder.Read(ssaoPassData.QuadMeshRef);

            RenderTargetDescriptor outputDesc;
            outputDesc.Renderer = this;
            outputDesc.ResolutionReference = &m_outputRenderTarget;
            outputDesc.Format = EColorFormat::R32_FLOAT;
            data.Output = builder.Create<RenderTargetResource>("BlurredSSAO", outputDesc);
         },
         [](const SSAOBlurPassData& data)
         {
            bool bSSAOEnabled = *(*data.SSAOEnabledRef->GetActual());
            if (bSSAOEnabled)
            {
               ID3D11DeviceContext& context = data.Renderer->GetImmediateContext();
               context.ClearState();
               context.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

               auto vertexShader = data.VertexShader->GetActual();
               auto pixelShader = data.PixelShader->GetActual();
               auto sampler = data.Sampler->GetActual();
               auto source = data.Source->GetActual();
               auto viewport = data.Viewport->GetActual();
               auto depthDisableState = data.DepthDisableState->GetActual();
               auto quadMesh = *data.QuadMeshRef->GetActual();
               auto output = data.Output->GetActual();

               /** Binds */
               vertexShader->Bind(context);
               pixelShader->Bind(context);
               sampler->Bind(context, 0);
               source->BindAsShaderResource(context, 0, EShaderType::PixelShader);
               viewport->Bind(context);
               depthDisableState->Bind(context);
               output->BindAsRenderTarget(context);

               quadMesh->Bind(context, 0);

               /** Render */
               context.DrawIndexed(quadMesh->GetIndexCount(), 0, 0);

               /** Unbinds */
               output->UnbindRenderTarget(context);
               source->UnbindShaderResource(context);
               sampler->Unbind(context);
               pixelShader->Unbind(context);
               vertexShader->Unbind(context);
            }
         });

      ssaoBlurPass->SetCullImmune(true);

      auto ssaoBlurPassData = ssaoBlurPass->GetData();

      /** Ambient-Emissive Pass */
      struct AmbientEmissivePassData : public RenderPassDataBase
      {
         ViewportResource* Viewport = nullptr;
         DepthStencilStateResource* DepthDisableState = nullptr;
         BlendStateResource* AdditiveBlendState = nullptr;
         SamplerResource* AnisoSampler = nullptr;
         SamplerResource* LinearClampSampler = nullptr;
         SamplerResource* SSAOSampler = nullptr;
         GBufferResource* GBuffer = nullptr;
         DynamicCubemapRefResource* IrradianceMapRef = nullptr;
         DynamicCubemapRefResource* PrefilteredMapRef = nullptr;
         RenderTargetRefResource* BrdfLUTRef = nullptr;
         CameraRefResource* CamRef = nullptr;
         ConstantBufferResource* ParamsBuffer = nullptr;
         BoolRefResource* SSAOEnabledRef = nullptr;
         RenderTargetResource* BlurredSSAO = nullptr;
         FloatRefResource* GlobalAOFactorRef = nullptr;
         MeshRefResource* QuadMeshRef = nullptr;
         RenderTargetResource* Output = nullptr;
      };

      auto ambientEmissivePassVSRes = 
         m_frameGraph.AddExternalPermanentResource("AmbientEmissivePassVertexShader", ShaderDescriptor(), m_ambientEmissivePassVS);
      auto ambientEmissivePassPSRes =
         m_frameGraph.AddExternalPermanentResource("AmbientEmissivePassPixelShader", ShaderDescriptor(), m_ambientEmissivePassPS);

      auto ambientEmissivePass = m_frameGraph.AddCallbackPass<AmbientEmissivePassData>(
         "AmbientEmissivePass",
         [&](Elaina::RenderPassBuilder& builder, AmbientEmissivePassData& data)
         {
            data.Renderer = this;
            data.VertexShader = builder.Read(ambientEmissivePassVSRes);
            data.PixelShader = builder.Read(ambientEmissivePassPSRes);

            data.Viewport = builder.Read(ssaoBlurPassData.Viewport);
            data.DepthDisableState = builder.Read(ssaoBlurPassData.DepthDisableState);
            data.AdditiveBlendState = builder.Read(lightingPassData.AdditiveBlendState);

            SamplerDescriptor anisoSamplerDesc;
            anisoSamplerDesc.Renderer = this;
            anisoSamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
            anisoSamplerDesc.AddressModeU = anisoSamplerDesc.AddressModeV = anisoSamplerDesc.AddressModeW = D3D11_TEXTURE_ADDRESS_WRAP;
            anisoSamplerDesc.CompFunc = D3D11_COMPARISON_ALWAYS;
            data.AnisoSampler = builder.Create<SamplerResource>("AnisoSampler", anisoSamplerDesc);

            SamplerDescriptor linClampSamplerDesc;
            linClampSamplerDesc.Renderer = this;
            linClampSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            linClampSamplerDesc.AddressModeU = linClampSamplerDesc.AddressModeV = linClampSamplerDesc.AddressModeW = D3D11_TEXTURE_ADDRESS_WRAP;
            linClampSamplerDesc.CompFunc = D3D11_COMPARISON_ALWAYS;
            data.LinearClampSampler = builder.Create<SamplerResource>("LinearClampSampler", linClampSamplerDesc);

            SamplerDescriptor ssaoSamplerDesc;
            ssaoSamplerDesc.Renderer = this;
            ssaoSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
            ssaoSamplerDesc.AddressModeU = ssaoSamplerDesc.AddressModeV = ssaoSamplerDesc.AddressModeW = D3D11_TEXTURE_ADDRESS_WRAP;
            ssaoSamplerDesc.CompFunc = D3D11_COMPARISON_ALWAYS;
            data.SSAOSampler = builder.Create<SamplerResource>("SSAOSampler", anisoSamplerDesc);

            data.GBuffer = builder.Read(geometryPassData.OutputGBuffer);
            data.IrradianceMapRef = builder.Read(diffuseIntegralPassData.OutputIrradianceMapRef);
            data.PrefilteredMapRef = builder.Read(prefilterEnvMapPassData.OutputPrefilteredEnvMapRef);
            data.BrdfLUTRef = builder.Read(integrateBRDFPassData.OutputBrdfLUTRef);
            data.CamRef = builder.Read(ssaoPassData.CamRef);

            ConstantBufferDescriptor paramsBufferDesc;
            paramsBufferDesc.Renderer = this;
            paramsBufferDesc.Size = sizeof(AmbientParamsConstantBuffer);
            data.ParamsBuffer = builder.Create<ConstantBufferResource>("AmbientEmissiveParamsConstantBuffer", paramsBufferDesc);
            data.SSAOEnabledRef = builder.Read(ssaoBlurPassData.SSAOEnabledRef);
            data.BlurredSSAO = builder.Read(ssaoBlurPassData.Output);

            FloatRefDescriptor globalAOFactorRefDesc;
            globalAOFactorRefDesc.Reference = &m_globalAOFactor;
            data.GlobalAOFactorRef = builder.Create<FloatRefResource>("GlobalAOFactorRef", globalAOFactorRefDesc);

            data.QuadMeshRef = builder.Read(ssaoBlurPassData.QuadMeshRef);

            data.Output = builder.Write(lightingPassData.OutputHDRBuffer);
         },
         [](const AmbientEmissivePassData& data)
         {
            ID3D11DeviceContext& context = data.Renderer->GetImmediateContext();
            context.ClearState();
            context.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            auto vertexShader = data.VertexShader->GetActual();
            auto pixelShader = data.PixelShader->GetActual();
            auto viewport = data.Viewport->GetActual();
            auto depthDisableState = data.DepthDisableState->GetActual();
            auto additiveBlendState = data.AdditiveBlendState->GetActual();
            auto anisoSampler = data.AnisoSampler->GetActual();
            auto linearClampSampler = data.LinearClampSampler->GetActual();
            auto ssaoSampler = data.SSAOSampler->GetActual();
            auto gBuffer = data.GBuffer->GetActual();
            auto irraidianceMap = *data.IrradianceMapRef->GetActual();
            auto prefilteredMap = *data.PrefilteredMapRef->GetActual();
            auto brdfLUT = *data.BrdfLUTRef->GetActual();
            auto camera = *data.CamRef->GetActual();
            bool bSSAOEnabled = *(*data.SSAOEnabledRef->GetActual());
            auto blurredSSAO = data.BlurredSSAO->GetActual();
            auto quadMesh = *data.QuadMeshRef->GetActual();
            auto output = data.Output->GetActual();

            /** Binds */
            vertexShader->Bind(context);
            pixelShader->Bind(context);
            viewport->Bind(context);
            depthDisableState->Bind(context);
            additiveBlendState->Bind(context);
            anisoSampler->Bind(context, 0);
            linearClampSampler->Bind(context, 1);
            ssaoSampler->Bind(context, 2);
            gBuffer->BindAsShaderResource(context, 0);
            irraidianceMap->Bind(context, 5, EShaderType::PixelShader);
            prefilteredMap->Bind(context, 6, EShaderType::PixelShader);
            brdfLUT->BindAsShaderResource(context, 7, EShaderType::PixelShader);
            if (bSSAOEnabled)
            {
               blurredSSAO->BindAsShaderResource(context, 8, EShaderType::PixelShader);
            }
            quadMesh->Bind(context, 0);
            output->BindAsRenderTarget(context, false, false);

            /** Update Constant Buffer */
            auto camTransform = camera->GetTransform();
            auto paramsBuffer = data.ParamsBuffer->GetActual();
            float globalAOFactor = *(*data.GlobalAOFactorRef->GetActual());
            auto mappedParamsBuffer = paramsBuffer->Map<AmbientParamsConstantBuffer>(context);
            (*mappedParamsBuffer) = AmbientParamsConstantBuffer{camTransform->GetPosition(TransformSpace::World), globalAOFactor, static_cast<unsigned int>(bSSAOEnabled)};
            paramsBuffer->UnMap(context);
            paramsBuffer->Bind(context, 0, EShaderType::PixelShader);

            /** Render */
            context.DrawIndexed(quadMesh->GetIndexCount(), 0, 0);

            /** Unbinds */
            output->UnbindRenderTarget(context);
            blurredSSAO->UnbindShaderResource(context);
            brdfLUT->UnbindShaderResource(context);
            prefilteredMap->Unbind(context);
            irraidianceMap->Unbind(context);
            gBuffer->UnbindShaderResource(context);
            ssaoSampler->Unbind(context);
            linearClampSampler->Unbind(context);
            anisoSampler->Unbind(context);
            pixelShader->Unbind(context);
            vertexShader->Unbind(context);
         });

      ambientEmissivePass->SetCullImmune(true);
      auto ambientEmissivePassData = ambientEmissivePass->GetData();

      struct SkyboxPassData : public RenderPassDataBase
      {
         ViewportResource* Viewport = nullptr;
         CameraRefResource* CamRef = nullptr;
         GBufferResource* GBuffer = nullptr;
         VoidRefRefResource* SkyboxType = nullptr;
         DynamicCubemapRefResource* EnvironmentMapRef = nullptr;
         DynamicCubemapRefResource* IrradianceMapRef = nullptr;
         SamplerResource* Sampler = nullptr;
         MeshRefResource* CubeMeshRef = nullptr;
         DepthStencilStateResource* DepthLessEqualState = nullptr;
         RasterizerStateResource* NoCullingState = nullptr;
         ConstantBufferResource* TransformConstantBuffer = nullptr;
         RenderTargetResource* Output = nullptr;
      };

      auto skyboxPassVSRes =
         m_frameGraph.AddExternalPermanentResource("SkyboxPassVertexShader", ShaderDescriptor(), m_skyboxPassVS);
      auto skyboxPassPSRes =
         m_frameGraph.AddExternalPermanentResource("SkyboxPassPixelShader", ShaderDescriptor(), m_skyboxPassPS);

      auto skyboxPass = m_frameGraph.AddCallbackPass<SkyboxPassData>(
         "SkyboxPass",
         [&](Elaina::RenderPassBuilder& builder, SkyboxPassData& data)
         {
            data.Renderer = this;
            data.VertexShader = builder.Read(skyboxPassVSRes);
            data.PixelShader = builder.Read(skyboxPassPSRes);

            data.Viewport = builder.Read(ambientEmissivePassData.Viewport);

            data.CamRef = builder.Read(ambientEmissivePassData.CamRef);
            data.GBuffer = builder.Read(ambientEmissivePassData.GBuffer);

            VoidRefDescriptor skyboxRefDesc;
            skyboxRefDesc.Reference = (void*)&m_renderSkyboxType;
            data.SkyboxType = builder.Create<VoidRefRefResource>("SkyboxType", skyboxRefDesc);

            SamplerDescriptor samplerDesc;
            samplerDesc.Renderer = this;
            samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            samplerDesc.AddressModeU = samplerDesc.AddressModeV = samplerDesc.AddressModeW = D3D11_TEXTURE_ADDRESS_WRAP;
            samplerDesc.CompFunc = D3D11_COMPARISON_ALWAYS;
            data.Sampler = builder.Create<SamplerResource>("SkyboxSampler", samplerDesc);

            data.DepthLessEqualState = builder.Read(convertSkyboxToCubemapPassData.DepthLessEqualState);
            data.NoCullingState = builder.Read(convertSkyboxToCubemapPassData.NoCullingState);

            ConstantBufferDescriptor transformBufferDesc;
            transformBufferDesc.Renderer = this;
            transformBufferDesc.Size = sizeof(OneMatrixConstantBuffer);
            data.TransformConstantBuffer = builder.Create<ConstantBufferResource>("SkyboxTransformConstantBuffer", transformBufferDesc);

            data.EnvironmentMapRef = builder.Read(convertSkyboxToCubemapPassData.OutputEnvMapRef);
            data.IrradianceMapRef = builder.Read(diffuseIntegralPassData.OutputIrradianceMapRef);

            data.CubeMeshRef = builder.Read(convertSkyboxToCubemapPassData.CubeMeshRef);

            data.Output = builder.Write(ambientEmissivePassData.Output);
         },
         [](const SkyboxPassData& data)
         {
            ID3D11DeviceContext& context = data.Renderer->GetImmediateContext();
            context.ClearState();
            context.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            auto vertexShader = data.VertexShader->GetActual();
            auto pixelShader = data.PixelShader->GetActual();
            auto sampler = data.Sampler->GetActual();
            auto viewport = data.Viewport->GetActual();
            auto depthLessEqualState = data.DepthLessEqualState->GetActual();
            auto noCullingState = data.NoCullingState->GetActual();
            auto transformBuffer = data.TransformConstantBuffer->GetActual();
            auto envMap = *data.EnvironmentMapRef->GetActual();
            auto irrdianceMap = *data.IrradianceMapRef->GetActual();
            auto cubeMesh = *data.CubeMeshRef->GetActual();
            auto output = data.Output->GetActual();
            auto gBuffer = data.GBuffer->GetActual();
            auto skyboxType = *reinterpret_cast<ESkyboxType*>(*data.SkyboxType->GetActual());

            /** Binds */
            vertexShader->Bind(context);
            pixelShader->Bind(context);
            sampler->Bind(context, 0);
            depthLessEqualState->Bind(context);
            noCullingState->Bind(context);
            transformBuffer->Bind(context, 0, EShaderType::VertexShader);
            cubeMesh->Bind(context, 0);
            viewport->Bind(context);
            auto gBufferDepthStencil = gBuffer->GetDepthStencilBufferDX11();
            output->SetDepthStencilBuffer(gBufferDepthStencil);
            output->BindAsRenderTarget(context, false, false);

            switch (skyboxType)
            {
            case ESkyboxType::IrradianceMap:
               irrdianceMap->Bind(context, 0, EShaderType::PixelShader);
               break;
            case ESkyboxType::EnvironmentMap:
            default:
               envMap->Bind(context, 0, EShaderType::PixelShader);
            }

            /** Upload Constant Buffer datas */
            auto camera = *data.CamRef->GetActual();
            auto camTransform = camera->GetTransform();
            Matrix viewMat = Matrix::CreateView(Vector3(0.0f, 0.0f, 0.0f), camTransform->GetForward(TransformSpace::World), camTransform->GetUp(TransformSpace::World));
            Matrix projMat = Matrix::CreatePerspectiveProj(camera->GetFov(), output->GetAspectRatio(), 0.1f, 1000.0f);
            auto mappedTransformBuffer = transformBuffer->Map<OneMatrixConstantBuffer>(context);
            (*mappedTransformBuffer) = OneMatrixConstantBuffer{ viewMat * projMat };
            transformBuffer->UnMap(context);

            /** Render */
            context.DrawIndexed(cubeMesh->GetIndexCount(), 0, 0);

            /** Unbinds */
            switch (skyboxType)
            {
            case ESkyboxType::IrradianceMap:
               irrdianceMap->Unbind(context);
               break;
            case ESkyboxType::EnvironmentMap:
            default:
               envMap->Unbind(context);
               break;
            }

            output->UnbindRenderTarget(context);
            output->SetDepthStencilBuffer(nullptr);
            transformBuffer->Unbind(context);
            sampler->Unbind(context);
            pixelShader->Unbind(context);
            vertexShader->Unbind(context);
         });

      skyboxPass->SetCullImmune(true);
      m_frameGraph.Compile();

      Elaina::VisualizeParams visualizeParams;
      visualizeParams.bSplines = true;
      visualizeParams.RankSep = 3.0;
      m_frameGraph.ExportVisualization("RendererPBR.dot", visualizeParams);
      return true;
   }

   bool RendererPBR::SetupSSAOParams()
   {
      if (m_outputRenderTarget != nullptr)
      {
         unsigned int width = m_outputRenderTarget->GetWidth();
         unsigned int height = m_outputRenderTarget->GetHeight();
         m_ssaoParams.NoiseScale = Vector2(width / (float)RendererPBRConstants::SSAONoiseTextureSize, height / (float)RendererPBRConstants::SSAONoiseTextureSize);

         std::uniform_real_distribution<float> randomFloats(0.0f, 1.0f);
         std::default_random_engine generator;
         for (unsigned int idx = 0; idx < RendererPBRConstants::SSAOKernelSize; ++idx)
         {
            Vector4 sample{
               randomFloats(generator) * 2.0f - 1.0f,
               randomFloats(generator) * 2.0f - 1.0f,
               randomFloats(generator) /* Hemisphere : z = [0, 1] */,
               0.0f };
            sample.Normalize();
            sample *= randomFloats(generator);

            float scale = (float)idx / (float)RendererPBRConstants::SSAOKernelSize;
            scale = Math::Lerp(0.1f, 1.0f, scale * scale);
            sample *= scale;

            m_ssaoParams.Samples[idx] = sample;
         }

         size_t noiseTexRes2 = (RendererPBRConstants::SSAONoiseTextureSize * RendererPBRConstants::SSAONoiseTextureSize);
         for (size_t idx = 0; idx < noiseTexRes2; ++idx)
         {
            m_ssaoParams.Noise[idx] = Vector4(
               randomFloats(generator) * 2.0f - 1.0f,
               randomFloats(generator) * 2.0f - 1.0f,
               0.0f,
               0.0f);
         }

         return true;
      }

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

         if (!SetupSSAOParams())
         {
            ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to setup ssao params!"));
         }

         m_frameGraph.Execute();
      }
   }
}