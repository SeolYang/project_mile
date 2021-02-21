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
#include "Rendering/GPUProfiler.h"
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
      Matrix WorldMatrix = Matrix::Identity;
      Matrix WorldViewMatrix = Matrix::Identity;
      Matrix WorldViewProjMatrix = Matrix::Identity;
   };

   DEFINE_CONSTANT_BUFFER(PackedMaterialParams)
   {
      Vector4 BaseColorFactor = Vector4::One();
      Vector4 EmissiveColorFactor = Vector4::Zero();
      Vector4 MetallicRoughnessUV = Vector4::Zero();
      float SpecularFactor = 0.0f;
   };

   DEFINE_CONSTANT_BUFFER(OneFloatConstantBuffer)
   {
      float Value = 0.0f;
   };

   DEFINE_CONSTANT_BUFFER(OneUINTConstantBuffer)
   {
      unsigned int Value = 0;
   };

   DEFINE_CONSTANT_BUFFER(OneVector2ConstantBuffer)
   {
      Vector2 Value = Vector2();
   };

   DEFINE_CONSTANT_BUFFER(OneVector3ConstantBuffer)
   {
      Vector3 Value = Vector3();
   };

   DEFINE_CONSTANT_BUFFER(LightParamsConstantBuffer)
   {
      Vector4 LightPosIntensity = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
      Vector4 LightDirectionRadius = Vector4(0.0f, -1.0f, 0.0f, 1.0f);
      Vector3 LightColor = Vector3(0.0f, 0.0f, 0.0f);
      Vector2 LightAngles = Vector2(15.f, 15.f);
      UINT32 LightType = 0;
   };

   DEFINE_CONSTANT_BUFFER(OneMatrixConstantBuffer)
   {
      Matrix Mat = Matrix::Identity;
   };

   DEFINE_CONSTANT_BUFFER(SSAOParamsConstantBuffer)
   {
      Matrix Projection = Matrix::Identity;
      Vector3 PackedParams = Vector3(1.0f, 0.0f, 1.0f); // Rad, Bias, Mag
   };

   DEFINE_CONSTANT_BUFFER(AmbientParamsConstantBuffer)
   {
      Vector3 CameraPos = Vector3();
      float AmbientIntensity = 1.0f;
      unsigned int SSAOEnabled = 0;
   };

   DEFINE_CONSTANT_BUFFER(DebugDepthSSAOConstantBuffer)
   {
      unsigned int Type = 0;
      Vector2 nearFar = Vector2(0.1f, 1000.0f);
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
      m_ambientIntensity(1.0f),
      m_skyboxPassVS(nullptr),
      m_skyboxPassPS(nullptr),
      m_renderSkyboxType(ESkyboxType::EnvironmentMap),
      m_extractBrightnessPassVS(nullptr),
      m_extractBrightnessPassPS(nullptr),
      m_gaussBloomPassVS(nullptr),
      m_gaussBloomPassPS(nullptr),
      m_printTextureVS(nullptr),
      m_printTexturePS(nullptr),
      m_toneMappingVS(nullptr),
      m_toneMappingPS(nullptr),
      m_gBuffer(nullptr),
      m_hdrBuffer(nullptr),
      m_extractedBrightness(nullptr),
      m_ssao(nullptr),
      m_ssaoBaseDataBuffer(nullptr),
      m_ssaoNoiseTex(nullptr),
      m_blurredSSAO(nullptr),
      m_pingPongBuffers({ nullptr, }),
      m_depthDebugBuffer(nullptr),
      m_ssaoDebugBuffer(nullptr),
      m_debugDepthSSAOVS(nullptr),
      m_debugDepthSSAOPS(nullptr),
      m_lightingDebugBuffer(nullptr)
   {
   }

   RendererPBR::~RendererPBR()
   {
      m_frameGraph.Clear();
      SafeDelete(m_lightingDebugBuffer);
      SafeDelete(m_depthDebugBuffer);
      SafeDelete(m_ssaoDebugBuffer);
      SafeDelete(m_ssaoNoiseTex);
      SafeDelete(m_ssaoBaseDataBuffer);
      SafeDelete(m_blurredSSAO);
      SafeDelete(m_ssao);
      for (auto* pingPongBuffer : m_pingPongBuffers)
      {
         SafeDelete(pingPongBuffer);
      }
      SafeDelete(m_extractedBrightness);
      SafeDelete(m_hdrBuffer);
      SafeDelete(m_gBuffer);
      SafeDelete(m_debugDepthSSAOPS);
      SafeDelete(m_debugDepthSSAOVS);
      SafeDelete(m_toneMappingPS);
      SafeDelete(m_toneMappingVS);
      SafeDelete(m_printTexturePS);
      SafeDelete(m_printTextureVS);
      SafeDelete(m_gaussBloomPassPS);
      SafeDelete(m_gaussBloomPassVS);
      SafeDelete(m_extractBrightnessPassPS);
      SafeDelete(m_extractBrightnessPassVS);
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
      SafeDelete(m_prefilterEnvPassPS);
      SafeDelete(m_prefilterEnvPassVS);
      SafeDelete(m_irradianceMap);
      SafeDelete(m_diffuseIntegralPassPS);
      SafeDelete(m_diffuseIntegralPassVS);
      SafeDelete(m_environmentMap);
      SafeDelete(m_convertSkyboxPassPS);
      SafeDelete(m_convertSkyboxPassVS);
      SafeDelete(m_geometryPassPS);
      SafeDelete(m_geometryPassVS);
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

      /** Extract Brightness Pass Shaders  */
      ShaderDescriptor extractBrightnessPassDesc;
      extractBrightnessPassDesc.Renderer = this;
      extractBrightnessPassDesc.FilePath = TEXT("Contents/Shaders/ExtractBrightness.hlsl");
      m_extractBrightnessPassVS = Elaina::Realize<ShaderDescriptor, VertexShaderDX11>(extractBrightnessPassDesc);
      if (m_extractBrightnessPassVS == nullptr)
      {
         ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to load extract brightness pass vertex shader!"));
         return false;
      }

      m_extractBrightnessPassPS = Elaina::Realize<ShaderDescriptor, PixelShaderDX11>(extractBrightnessPassDesc);
      if (m_extractBrightnessPassPS == nullptr)
      {
         ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to load extract brightness pass pixel shader!"));
         return false;
      }

      /** Gaussian Bloom Pass Shaders  */
      ShaderDescriptor gaussBloomPassDesc;
      gaussBloomPassDesc.Renderer = this;
      gaussBloomPassDesc.FilePath = TEXT("Contents/Shaders/GaussianBlur.hlsl");
      m_gaussBloomPassVS = Elaina::Realize<ShaderDescriptor, VertexShaderDX11>(gaussBloomPassDesc);
      if (m_gaussBloomPassVS == nullptr)
      {
         ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to load gaussian bloom pass vertex shader!"));
         return false;
      }

      m_gaussBloomPassPS = Elaina::Realize<ShaderDescriptor, PixelShaderDX11>(gaussBloomPassDesc);
      if (m_gaussBloomPassPS == nullptr)
      {
         ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to load gaussian bloom pass pixel shader!"));
         return false;
      }

      /** Print Texture Pass Shaders  */
      ShaderDescriptor printTextureDesc;
      printTextureDesc.Renderer = this;
      printTextureDesc.FilePath = TEXT("Contents/Shaders/PrintTexture.hlsl");
      m_printTextureVS = Elaina::Realize<ShaderDescriptor, VertexShaderDX11>(printTextureDesc);
      if (m_printTextureVS == nullptr)
      {
         ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to load print texture vertex shader!"));
         return false;
      }

      m_printTexturePS = Elaina::Realize<ShaderDescriptor, PixelShaderDX11>(printTextureDesc);
      if (m_printTexturePS == nullptr)
      {
         ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to load print texture pixel shader!"));
         return false;
      }

      /** Tone Mapping Pass Shaders  */
      ShaderDescriptor toneMappingDesc;
      toneMappingDesc.Renderer = this;
      toneMappingDesc.FilePath = TEXT("Contents/Shaders/ToneMapping.hlsl");
      m_toneMappingVS = Elaina::Realize<ShaderDescriptor, VertexShaderDX11>(toneMappingDesc);
      if (m_toneMappingVS == nullptr)
      {
         ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to load tone mapping vertex shader!"));
         return false;
      }

      m_toneMappingPS = Elaina::Realize<ShaderDescriptor, PixelShaderDX11>(toneMappingDesc);
      if (m_toneMappingPS == nullptr)
      {
         ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to load tone mapping pixel shader!"));
         return false;
      }

      /** Debug Depth-SSAO Pass Shaders  */
      ShaderDescriptor debugDepthSsaoDesc;
      debugDepthSsaoDesc.Renderer = this;
      debugDepthSsaoDesc.FilePath = TEXT("Contents/Shaders/DebugDepthSSAO.hlsl");
      m_debugDepthSSAOVS = Elaina::Realize<ShaderDescriptor, VertexShaderDX11>(debugDepthSsaoDesc);
      if (m_debugDepthSSAOVS == nullptr)
      {
         ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to load depth ssao debug vertex shader!"));
         return false;
      }

      m_debugDepthSSAOPS = Elaina::Realize<ShaderDescriptor, PixelShaderDX11>(debugDepthSsaoDesc);
      if (m_debugDepthSSAOPS == nullptr)
      {
         ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to load depth ssao debug pixel shader!"));
         return false;
      }

      return true;
   }

   bool RendererPBR::InitFrameGraph()
   {
      SetupRenderResources();
      SetupSSAOParams();

      auto targetCameraRefRes = m_frameGraph.AddExternalPermanentResource("CameraRef", CameraRefDescriptor(), &m_targetCamera);
      auto lightsRes = m_frameGraph.AddExternalPermanentResource("Lights", WorldDescriptor(), &m_lights);
      auto meshesRes = m_frameGraph.AddExternalPermanentResource("Meshes", WorldDescriptor(), &m_meshes);
      auto materialMapRes = m_frameGraph.AddExternalPermanentResource("MaterialMap", WorldDescriptor(), &m_materialMap);
      auto outputRenderTargetRefRes = m_frameGraph.AddExternalPermanentResource("FinalOutputRef", RenderTargetRefDescriptor(), &m_outputRenderTarget);

      /** Geometry Pass */
      auto geometryPassVS = m_frameGraph.AddExternalPermanentResource("GeometryPassVS", ShaderDescriptor(), m_geometryPassVS);
      auto geometryPassPS = m_frameGraph.AddExternalPermanentResource("GeometryPassPS", ShaderDescriptor(), m_geometryPassPS);

      struct GeometryPassData : public RenderPassDataBase
      {
         CameraRefResource* TargetCameraRef = nullptr;
         MaterialMapResource* MaterialMap = nullptr;
         SamplerResource* Sampler = nullptr;
         std::vector<ConstantBufferResource*> TransformBuffers = { nullptr, };
         std::vector<ConstantBufferResource*> MaterialBuffers = { nullptr, };
         ViewportResource* OutputViewport = nullptr;
         RasterizerStateResource* OutputRasterizerState = nullptr;
         GBufferRefResource* OutputGBufferRef = nullptr;
      };

      auto gBufferRefRes = m_frameGraph.AddExternalPermanentResource("GBufferRef", GBufferRefDescriptor(), &m_gBuffer);
      auto geometryPass = m_frameGraph.AddCallbackPass<GeometryPassData>(
         "Geometry Pass",
         [&](Elaina::RenderPassBuilder& builder, GeometryPassData& data)
         {
            data.Renderer = this;
            data.VertexShader = builder.Read(geometryPassVS);
            data.PixelShader = builder.Read(geometryPassPS);

            SamplerDescriptor samplerDesc;
            samplerDesc.Renderer = this;
            data.Sampler = builder.Create<SamplerResource>("AnisoWrapAlwaysSampler", samplerDesc);

            data.TargetCameraRef = builder.Read(targetCameraRefRes);
            data.MaterialMap = builder.Read(materialMapRes);

            ConstantBufferDescriptor transformBufferDesc;
            transformBufferDesc.Renderer = this;
            transformBufferDesc.Size = sizeof(GeometryPassTransformBuffer);

            ConstantBufferDescriptor packedMaterialParamsBufferDesc;
            packedMaterialParamsBufferDesc.Renderer = this;
            packedMaterialParamsBufferDesc.Size = sizeof(PackedMaterialParams);

            size_t maximumThreadsNum = this->GetMaximumThreads();
            data.TransformBuffers.resize(maximumThreadsNum);
            data.MaterialBuffers.resize(maximumThreadsNum);
            for (size_t threads = 0; threads < maximumThreadsNum; ++threads)
            {
               data.TransformBuffers[threads] = builder.Create<ConstantBufferResource>("GeometryPassTransformConstantBuffers", transformBufferDesc);
               data.MaterialBuffers[threads] = builder.Create<ConstantBufferResource>("GeometryPassPackedMaterialParamsConstantBuffers", packedMaterialParamsBufferDesc);
            }

            data.OutputGBufferRef = builder.Write(gBufferRefRes);

            ViewportDescriptor viewportDesc;
            viewportDesc.Renderer = this;
            viewportDesc.OutputRenderTargetReference = &m_outputRenderTarget;
            data.OutputViewport = builder.Create<ViewportResource>("Viewport", viewportDesc);

            RasterizerStateDescriptor rasterizerStateDesc;
            rasterizerStateDesc.Renderer = this;
            data.OutputRasterizerState = builder.Create<RasterizerStateResource>("DefaultRasterizerState", rasterizerStateDesc);
         },
         [](const GeometryPassData& data)
         {
            OPTICK_EVENT("ExecuteGeometryPass");
            auto& profiler = data.Renderer->GetProfiler();
            auto vertexShader = data.VertexShader->GetActual();
            auto pixelShader = data.PixelShader->GetActual();
            auto sampler = data.Sampler->GetActual();
            auto gBuffer = *data.OutputGBufferRef->GetActual();
            auto rasterizerState = data.OutputRasterizerState->GetActual();
            auto viewport = data.OutputViewport->GetActual();
            auto targetCamera = (*data.TargetCameraRef->GetActual());
            auto& materialMap = *data.MaterialMap->GetActual();

            auto threadPool = Engine::GetThreadPool();
            size_t maximumThreadsNum = data.Renderer->GetMaximumThreads();

            // @For performance test!
            //RendererPBR::RenderMeshes(data.Renderer, true, *meshes, 0, meshesNum, data.Renderer->GetImmediateContext(), vertexShader, pixelShader, sampler, gBuffer, data.TransformBuffers[0]->GetActual(), data.MaterialBuffers[0]->GetActual(), rasterizerState, viewport, targetCamera);

            /** Scheduling */
            /** @todo 임시 구현이므로 좀더 나은 memory management 방법을 찾아서 구현하기 */
            constexpr size_t UpperBoundOfThreadsNum = 16;

            std::mutex mutexes[UpperBoundOfThreadsNum];
            size_t matSwitchingCounts[UpperBoundOfThreadsNum] = { 0, };
            size_t threadDrawMeshCounts[UpperBoundOfThreadsNum] = { 0, };
            Meshes renderTasks[UpperBoundOfThreadsNum];

            std::queue<std::future<void>> scheduleTaskQueue;
            std::queue <std::pair<size_t, std::future<void>>> renderTaskQueue;
            for (auto& mapData : materialMap)
            {
               auto& targetMeshes = mapData.second;
               if (targetMeshes.size() > 0)
               {
                  size_t tasksNum = 0;
                  size_t offset = 0;
                  size_t remain = targetMeshes.size() % maximumThreadsNum;
                  while (offset < targetMeshes.size())
                  {
                     size_t num = targetMeshes.size() / maximumThreadsNum;
                     if (remain > 0)
                     {
                        ++num;
                        --remain;
                     }

                     size_t minMatSwitchingCount = std::numeric_limits<size_t>::max();
                     size_t minMatSwitchingSubThreadIdx = 0;
                     for (size_t subThreadIdx = 0; subThreadIdx < maximumThreadsNum; ++subThreadIdx)
                     {
                        size_t switchingCount = matSwitchingCounts[subThreadIdx];
                        if (switchingCount == 0)
                        {
                           minMatSwitchingSubThreadIdx = subThreadIdx;
                           break;
                        }
                        else if (switchingCount < minMatSwitchingCount)
                        {
                           minMatSwitchingCount = switchingCount;
                           minMatSwitchingSubThreadIdx = subThreadIdx;
                        }
                        else if (switchingCount == minMatSwitchingCount)
                        {
                           if (threadDrawMeshCounts[subThreadIdx] < threadDrawMeshCounts[minMatSwitchingSubThreadIdx])
                           {
                              minMatSwitchingSubThreadIdx = subThreadIdx;
                           }
                        }
                     }

                     ++matSwitchingCounts[minMatSwitchingSubThreadIdx];
                     threadDrawMeshCounts[minMatSwitchingSubThreadIdx] += num;

                     scheduleTaskQueue.push(std::move(threadPool->AddTask([&targetMeshes, &renderTasks, &mutexes, minMatSwitchingSubThreadIdx, offset, num]()
                        {
                           OPTICK_EVENT("SchedulingGeometryPassRenderTask");
                           {
                              std::lock_guard<std::mutex> guard{ mutexes[minMatSwitchingSubThreadIdx] };
                              auto& renderTaskMeshes = renderTasks[minMatSwitchingSubThreadIdx];
                              std::copy_n(targetMeshes.begin() + offset, num, std::back_inserter(renderTaskMeshes));
                           }
                        })));

                     offset += num;
                  }
               }
            }

            while (!scheduleTaskQueue.empty())
            {
               auto task{ std::move(scheduleTaskQueue.front()) };
               scheduleTaskQueue.pop();
               task.get();
            }

            /** Meshes */
            for (size_t subThreadIdx = 0; subThreadIdx < maximumThreadsNum; ++subThreadIdx)
            {
               if (renderTasks[subThreadIdx].size() > 0)
               {
                  auto& profiler = data.Renderer->GetProfiler();
                  size_t threadIdx = subThreadIdx + 1; /** thread index = thread + 1(Main Thread) */
                  std::string taskName = "GeometryPass_thread";
                  taskName.append(std::to_string(threadIdx));

                  auto transformBuffer = data.TransformBuffers[subThreadIdx]->GetActual();
                  auto materialParamsBuffer = data.MaterialBuffers[subThreadIdx]->GetActual();

                  auto& renderTask = renderTasks[subThreadIdx];
                  renderTaskQueue.push(std::make_pair(subThreadIdx, threadPool->AddTask([=, &profiler, &renderTask]()
                     {
                        OPTICK_EVENT("ExecuteGeometryPassRenderTask");
                        ScopedDeferredGPUProfile deferredProfile{ profiler, taskName, data.Renderer->GetDeferredContext(subThreadIdx) };
                        RendererPBR::RenderMeshes(
                           data.Renderer,
                           false, renderTask, 0, renderTask.size(),
                           vertexShader, pixelShader, sampler,
                           gBuffer, transformBuffer, materialParamsBuffer,
                           rasterizerState, viewport, targetCamera, threadIdx);
                     })));
               }
            }

            profiler.Begin("GeometryPass");
            /** Clear GBuffer */
            ID3D11DeviceContext& immediateContext = data.Renderer->GetImmediateContext();
            gBuffer->BindAsRenderTarget(immediateContext);
            gBuffer->UnbindRenderTarget(immediateContext);
            while (!renderTaskQueue.empty())
            {
               auto renderTask{ std::move(renderTaskQueue.front()) };
               renderTaskQueue.pop();
               renderTask.second.get();

               ID3D11DeviceContext& deferredContext = data.Renderer->GetDeferredContext(renderTask.first);

               ID3D11CommandList* commandList = nullptr;
               deferredContext.FinishCommandList(false, &commandList);
               if (commandList != nullptr)
               {
                  immediateContext.ClearState();
                  immediateContext.ExecuteCommandList(commandList, false);
               }

               SafeRelease(commandList);
            }
            profiler.End("GeometryPass");
         });

      const auto& geometryPassData = geometryPass->GetData();

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

            data.IBLPrecomputeEnabledRef = builder.Create<BoolRefResource>("IBLPrecomputeEnabledRef", BoolRefDescriptor{ &m_bPrecomputeIBL });
            data.SkyboxTextureRef = builder.Read(skyboxTextureRefRes);
            for (size_t idx = 0; idx < CUBE_FACES; ++idx)
            {
               data.CaptureViews[idx] = builder.Create<MatrixResource>("CubeCaptureViews", MatrixDescriptor{ captureViews[idx] });
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
            OPTICK_EVENT("ExecuteConvertSkyboxPass");
            auto& profiler = data.Renderer->GetProfiler();
            ScopedGPUProfile profile(profiler, "ConvertSkyboxPass");

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
                  data.Renderer->DrawIndexed(cubeMesh->GetVertexCount(), cubeMesh->GetIndexCount());
                  outputEnvMap->UnbindAsRenderTarget(immediateContext);
               }
               outputEnvMap->GenerateMips(immediateContext);

               /** Unbinds */
               if (skyboxTextureDX11 != nullptr)
               {
                  skyboxTextureDX11->Unbind(immediateContext, 0, EShaderType::PixelShader);
               }
               sampler->Unbind(immediateContext, 0);
               pixelShader->Unbind(immediateContext);
               transformBuffer->Unbind(immediateContext, 0, EShaderType::VertexShader);
               vertexShader->Unbind(immediateContext);
            }
         });

      const auto& convertSkyboxToCubemapPassData = convertSkyboxToCubemapPass->GetData();

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
            OPTICK_EVENT("ExecuteDiffuseIntegralPass");
            auto& profiler = data.Renderer->GetProfiler();
            ScopedGPUProfile profile(profiler, "DiffuseIntegralPass");

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
                  data.Renderer->DrawIndexed(cubeMesh->GetVertexCount(), cubeMesh->GetIndexCount());
                  outputIrradianceMap->UnbindAsRenderTarget(immediateContext);
               }
               outputIrradianceMap->GenerateMips(immediateContext);

               /** Unbinds */
               envMap->Unbind(immediateContext, 0, EShaderType::PixelShader);
               transformBuffer->Unbind(immediateContext, 0, EShaderType::VertexShader);
               sampler->Unbind(immediateContext, 0);
               pixelShader->Unbind(immediateContext);
               vertexShader->Unbind(immediateContext);
            }
         });

      const auto& diffuseIntegralPassData = diffuseIntegralPass->GetData();

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
            paramsBufferDesc.Size = sizeof(OneFloatConstantBuffer);
            data.PrefilterParamsBuffer = builder.Create<ConstantBufferResource>("OneFlaotConstantBuffer", paramsBufferDesc);

            data.EnvironmentMapRef = builder.Read(diffuseIntegralPassData.EnvironmentMapRef);

            data.CubeMeshRef = builder.Read(diffuseIntegralPassData.CubeMeshRef);
            data.OutputPrefilteredEnvMapRef = builder.Write(prefilteredEnvMapRefRes);
         },
         [](const PrefilterEnvPassData& data)
         {
            OPTICK_EVENT("ExecutePrefilterEnvPass");
            auto& profiler = data.Renderer->GetProfiler();
            ScopedGPUProfile profile(profiler, "PrefilterEnvironmentMapPass");

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
                  auto mappedParamsBuffer = paramsBuffer->Map<OneFloatConstantBuffer>(immediateContext);
                  (*mappedParamsBuffer) = OneFloatConstantBuffer{ roughness };
                  paramsBuffer->UnMap(immediateContext);

                  for (unsigned int faceIdx = 0; faceIdx < CUBE_FACES; ++faceIdx)
                  {
                     auto mappedTrasnformBuffer = transformBuffer->Map<OneMatrixConstantBuffer>(immediateContext);
                     (*mappedTrasnformBuffer) = OneMatrixConstantBuffer{ *data.CaptureViews[faceIdx]->GetActual() };
                     transformBuffer->UnMap(immediateContext);

                     outputPrefilteredEnvMap->BindAsRenderTarget(immediateContext, faceIdx, mipLevel);
                     data.Renderer->DrawIndexed(cubeMesh->GetVertexCount(), cubeMesh->GetIndexCount());
                     outputPrefilteredEnvMap->UnbindAsRenderTarget(immediateContext);
                  }
               }

               /** Unbinds */
               envMap->Unbind(immediateContext, 0, EShaderType::PixelShader);
               paramsBuffer->Unbind(immediateContext, 0, EShaderType::PixelShader);
               transformBuffer->Unbind(immediateContext, 0, EShaderType::VertexShader);
               sampler->Unbind(immediateContext, 0);
               pixelShader->Unbind(immediateContext);
               vertexShader->Unbind(immediateContext);
            }
         });

      const auto& prefilterEnvMapPassData = prefilterEnvMapPass->GetData();

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
            OPTICK_EVENT("ExecuteIntegrateBRDFPass");
            auto& profiler = data.Renderer->GetProfiler();
            ScopedGPUProfile profile(profiler, "IntegrateBRDFPass");

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

               viewport->SetWidth((float)outputBrdfLUT->GetWidth());
               viewport->SetHeight((float)outputBrdfLUT->GetHeight());

               /** Binds */
               vertexShader->Bind(immediateContext);
               pixelShader->Bind(immediateContext);
               sampler->Bind(immediateContext, 0);
               depthDisableState->Bind(immediateContext);
               viewport->Bind(immediateContext);
               quadMesh->Bind(immediateContext, 0);
               outputBrdfLUT->Clear(immediateContext, Vector4(0.0f, 0.0f, 0.0f, 0.0f));
               outputBrdfLUT->BindAsRenderTarget(immediateContext);

               /** Render */
               data.Renderer->DrawIndexed(quadMesh->GetVertexCount(), quadMesh->GetIndexCount());

               /** Unbinds */
               outputBrdfLUT->UnbindRenderTarget(immediateContext);
               sampler->Unbind(immediateContext, 0);
               pixelShader->Unbind(immediateContext);
               vertexShader->Unbind(immediateContext);

               (*bIBLPrecomputeEnabledRef) = false;
            }
         });

      const auto& integrateBRDFPassData = integrateBRDFPass->GetData();

      /** Lighting Pass; Deferred Shading - Lighting */
      struct LightingPassData : public RenderPassDataBase
      {
         VertexShaderResource* DebugVertexShader = nullptr;
         PixelShaderResource* DebugPixelShader = nullptr;

         SamplerResource* Sampler = nullptr;

         CameraRefResource* CamRef = nullptr;
         LightsDataResource* LightsData = nullptr;
         GBufferRefResource* GBufferRef = nullptr;
         ViewportResource* Viewport = nullptr;
         ConstantBufferResource* CameraParamsBuffer = nullptr;
         ConstantBufferResource* LightParamsBuffer = nullptr;
         DepthStencilStateResource* DepthDisableState = nullptr;
         BlendStateResource* AdditiveBlendState = nullptr;

         MeshRefResource* QuadMeshRef = nullptr;
         RenderTargetRefResource* OutputRef = nullptr;
         ConstantBufferResource* DebugParamsBuffer = nullptr;
         RenderTargetRefResource* DebugOutputRef = nullptr;
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

      auto printTextureVSRes = m_frameGraph.AddExternalPermanentResource("PrintTextureVertexShader", ShaderDescriptor(), m_printTextureVS);
      auto printTexturePSRes = m_frameGraph.AddExternalPermanentResource("PrintTexturePixelShader", ShaderDescriptor(), m_printTexturePS);

      auto hdrBufferRefRes = m_frameGraph.AddExternalPermanentResource("HDRBufferRef", RenderTargetRefDescriptor(), &m_hdrBuffer);
      auto lightingDebugBufferRefRes = m_frameGraph.AddExternalPermanentResource("LightingDebugBufferRef", RenderTargetRefDescriptor(), &m_lightingDebugBuffer);

      auto lightingPass = m_frameGraph.AddCallbackPass<LightingPassData>(
         "LightingPass",
         [&](Elaina::RenderPassBuilder& builder, LightingPassData& data)
         {
            data.Renderer = this;
            data.VertexShader = builder.Read(lightingPassVSRes);
            data.PixelShader = builder.Read(lightingPassPSRes);

            data.DebugVertexShader = builder.Read(printTextureVSRes);
            data.DebugPixelShader = builder.Read(printTexturePSRes);
            data.DebugParamsBuffer = builder.Write(prefilterEnvMapPassData.PrefilterParamsBuffer);
            data.DebugOutputRef = builder.Write(lightingDebugBufferRefRes);

            data.CamRef = builder.Read(geometryPassData.TargetCameraRef);
            data.LightsData = builder.Read(lightsDataRes);

            SamplerDescriptor samplerDesc;
            samplerDesc.Renderer = this;
            samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
            samplerDesc.AddressModeU = samplerDesc.AddressModeV = samplerDesc.AddressModeW = D3D11_TEXTURE_ADDRESS_BORDER;
            samplerDesc.CompFunc = D3D11_COMPARISON_ALWAYS;
            data.Sampler = builder.Create<SamplerResource>("AnistropicBorderSampler", samplerDesc);

            data.GBufferRef = builder.Read(geometryPassData.OutputGBufferRef);
            data.Viewport = builder.Read(geometryPassData.OutputViewport);

            ConstantBufferDescriptor cameraParamsDesc;
            cameraParamsDesc.Renderer = this;
            cameraParamsDesc.Size = sizeof(OneVector3ConstantBuffer);
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
            data.OutputRef = builder.Write(hdrBufferRefRes);
         },
         [](const LightingPassData& data)
         {
            OPTICK_EVENT("ExecuteLightingPass");
            auto& profiler = data.Renderer->GetProfiler();
            ScopedGPUProfile profile(profiler, "LightingPass");
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
            auto gBuffer = *data.GBufferRef->GetActual();
            auto viewport = data.Viewport->GetActual();
            auto quadMesh = *data.QuadMeshRef->GetActual();
            auto outputHDRBuffer = *data.OutputRef->GetActual();

            /** Binds */
            vertexShader->Bind(immediateContext);
            pixelShader->Bind(immediateContext);
            sampler->Bind(immediateContext, 0);
            depthDisableState->Bind(immediateContext);
            additiveBlendState->Bind(immediateContext);
            camParamsBuffer->Bind(immediateContext, 0, EShaderType::PixelShader);
            lightParamsBuffer->Bind(immediateContext, 1, EShaderType::PixelShader);
            gBuffer->BindAsShaderResource(immediateContext, 0, EShaderType::PixelShader);
            viewport->Bind(immediateContext);
            quadMesh->Bind(immediateContext, 0);
            outputHDRBuffer->Clear(immediateContext, Vector4(0.0f, 0.0f, 0.0f, 1.0f));
            outputHDRBuffer->BindAsRenderTarget(immediateContext);

            /** Render */
            Transform* camTransform = camera->GetTransform();
            for (const auto lightComponent : lightsData)
            {
               auto mappedCamParamsBuffer = camParamsBuffer->Map<OneVector3ConstantBuffer>(immediateContext);
               (*mappedCamParamsBuffer) = OneVector3ConstantBuffer{ camTransform->GetPosition(ETransformSpace::World) };
               camParamsBuffer->UnMap(immediateContext);

               Transform* lightTransform = lightComponent->GetTransform();
               Vector3 lightPosition = lightTransform->GetPosition(ETransformSpace::World);
               Vector3 lightDirection = lightTransform->GetForward(ETransformSpace::World);
               Vector3 lightRadiance = lightComponent->GetColor();
               float lightIntensity = lightComponent->GetIntensity();
               float lightRadius = lightComponent->GetRadius();
               float lightInnerAngle = lightComponent->GetInnerAngleAsRadians();
               float lightOuterAngle = lightComponent->GetOuterAngleAsRadians();
               auto mappedLightParamsBuffer = lightParamsBuffer->Map<LightParamsConstantBuffer>(immediateContext);
               (*mappedLightParamsBuffer) = LightParamsConstantBuffer
               {
                  Vector4(lightPosition.x, lightPosition.y, lightPosition.z, lightIntensity),
                  Vector4(lightDirection.x, lightDirection.y, lightDirection.z, lightRadius),
                  Vector3(lightRadiance.x, lightRadiance.y, lightRadiance.z),
                  Vector2(lightInnerAngle, lightOuterAngle),
                  static_cast<UINT32>(lightComponent->GetLightType())
               };
               lightParamsBuffer->UnMap(immediateContext);

               data.Renderer->DrawIndexed(quadMesh->GetVertexCount(), quadMesh->GetIndexCount());
            }

            /** Unbinds */
            outputHDRBuffer->UnbindRenderTarget(immediateContext);
            gBuffer->UnbindShaderResource(immediateContext, 0, EShaderType::PixelShader);
            lightParamsBuffer->Unbind(immediateContext, 1, EShaderType::PixelShader);
            camParamsBuffer->Unbind(immediateContext, 0, EShaderType::PixelShader);
            pixelShader->Unbind(immediateContext);
            vertexShader->Unbind(immediateContext);

            auto debugVertexShader = data.DebugVertexShader->GetActual();
            auto debugPixelShader = data.DebugPixelShader->GetActual();
            auto debugParamsBuffer = data.DebugParamsBuffer->GetActual();
            auto debugOutput = *data.DebugOutputRef->GetActual();

            debugVertexShader->Bind(immediateContext);
            debugPixelShader->Bind(immediateContext);
            debugParamsBuffer->Bind(immediateContext, 0, EShaderType::PixelShader);

            auto mappedDebugParamsBuffer = debugParamsBuffer->Map<OneFloatConstantBuffer>(immediateContext);
            (*mappedDebugParamsBuffer) = OneFloatConstantBuffer{1.0f};
            debugParamsBuffer->UnMap(immediateContext);

            debugOutput->Clear(immediateContext, Vector4(0.0f, 0.0f, 0.0f, 1.0f));
            debugOutput->BindAsRenderTarget(immediateContext);
            outputHDRBuffer->BindAsShaderResource(immediateContext, 0, EShaderType::PixelShader);
            data.Renderer->DrawIndexed(quadMesh->GetVertexCount(), quadMesh->GetIndexCount());
            outputHDRBuffer->UnbindShaderResource(immediateContext, 0, EShaderType::PixelShader);
            debugOutput->UnbindRenderTarget(immediateContext);

            sampler->Unbind(immediateContext, 0);
            debugParamsBuffer->Unbind(immediateContext, 0, EShaderType::PixelShader);
            debugPixelShader->Unbind(immediateContext);
            debugVertexShader->Unbind(immediateContext);
         });

      const auto& lightingPassData = lightingPass->GetData();

      /** Post-process Pass */
      struct ConvertGBufferPassData : public RenderPassDataBase
      {
         SamplerResource* Sampler = nullptr;
         CameraRefResource* CamRef = nullptr;
         ConstantBufferResource* ConvertParamsBuffer = nullptr;

         ViewportResource* Viewport = nullptr;
         MeshRefResource* QuadMeshRef = nullptr;
         DepthStencilStateResource* DepthDisableState = nullptr;

         GBufferRefResource* SourceGBufferRef = nullptr;
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
               "OneMatrixConstantBuffer",
               paramsDesc);

            data.Viewport = builder.Read(geometryPassData.OutputViewport);
            data.QuadMeshRef = builder.Read(integrateBRDFPassData.QuadMeshRef);
            data.DepthDisableState = builder.Read(integrateBRDFPassData.DepthDisableState);

            data.SourceGBufferRef = builder.Read(geometryPassData.OutputGBufferRef);

            GBufferDescriptor convertedGBufferDesc;
            convertedGBufferDesc.Renderer = this;
            convertedGBufferDesc.OutputRenderTargetReference = &m_outputRenderTarget;
            data.ConvertedGBuffer = builder.Create<GBufferResource>("ViewSpaceGBuffer", convertedGBufferDesc);
         },
         [](const ConvertGBufferPassData& data)
         {
            OPTICK_EVENT("ExecuteConvertGBufferPass");
            auto& profiler = data.Renderer->GetProfiler();
            ScopedGPUProfile profile(profiler, "ConvertGBufferPass");
            ID3D11DeviceContext& immediateContext = data.Renderer->GetImmediateContext();
            immediateContext.ClearState();
            immediateContext.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            auto vertexShader = data.VertexShader->GetActual();
            auto pixelShader = data.PixelShader->GetActual();
            auto sampler = data.Sampler->GetActual();
            auto convertParamsBuffer = data.ConvertParamsBuffer->GetActual();
            auto sourceGBuffer = *data.SourceGBufferRef->GetActual();
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
            sourceGBuffer->BindAsShaderResource(immediateContext, 0, EShaderType::PixelShader, true);
            convertedGBuffer->BindAsRenderTarget(immediateContext);

            /** Render */
            auto camTransform = camera->GetTransform();
            Matrix viewMatrix = Matrix::CreateView(
               camTransform->GetPosition(ETransformSpace::World),
               camTransform->GetForward(ETransformSpace::World),
               camTransform->GetUp(ETransformSpace::World));

            auto mappedConvertParams = convertParamsBuffer->Map<OneMatrixConstantBuffer>(immediateContext);
            (*mappedConvertParams) = OneMatrixConstantBuffer{ viewMatrix };
            convertParamsBuffer->UnMap(immediateContext);

            data.Renderer->DrawIndexed(quadMesh->GetVertexCount(), quadMesh->GetIndexCount());

            /** Unbinds */
            convertedGBuffer->UnbindRenderTarget(immediateContext);
            sourceGBuffer->UnbindShaderResource(immediateContext, 0, EShaderType::PixelShader, true);
            convertParamsBuffer->Unbind(immediateContext, 0, EShaderType::PixelShader);
            sampler->Unbind(immediateContext, 0);
            pixelShader->Unbind(immediateContext);
            vertexShader->Unbind(immediateContext);
         });

      const auto& convertGBufferPassData = gBufferConvertPass->GetData();

      /** SSAO */
      struct SSAOPassData : public RenderPassDataBase
      {
         BoolRefResource* SSAOEnabledRef = nullptr;
         SamplerResource* Sampler = nullptr;
         SamplerResource* NoiseSampler = nullptr;

         ViewportResource* Viewport = nullptr;
         DepthStencilStateResource* DepthDisableState = nullptr;

         CameraRefResource* CamRef = nullptr;

         ConstantBufferRefResource* SSAOBaseDataBuffer = nullptr;
         ConstantBufferResource* SSAOParamsBuffer = nullptr;

         Texture2dDX11RefResource* NoiseTextureRef = nullptr;
         GBufferRefResource* GBufferRef = nullptr;
         GBufferResource* ViewspaceGBuffer = nullptr;

         MeshRefResource* QuadMeshRef = nullptr;
         RenderTargetRefResource* OutputRef = nullptr;
      };

      auto ssaoPassVSRes = m_frameGraph.AddExternalPermanentResource(
         "SSAOPassVertexShader",
         ShaderDescriptor(),
         m_ssaoPassVS);

      auto ssaoPassPSRes = m_frameGraph.AddExternalPermanentResource(
         "SSAOPassPixelShader",
         ShaderDescriptor(),
         m_ssaoPassPS);

      auto ssaoBaseDataBufferRefRes = m_frameGraph.AddExternalPermanentResource("SSAOBaseDataConstantBufferRef", ConstantBufferRefDescriptor(), &m_ssaoBaseDataBuffer);

      auto ssaoOutputRefRes = m_frameGraph.AddExternalPermanentResource("SSAO", RenderTargetRefDescriptor(), &m_ssao);

      auto ssaoNoiseTexRefRes = m_frameGraph.AddExternalPermanentResource("SSAONoiseTexRef", Texture2dDX11RefDescriptor(), &m_ssaoNoiseTex);

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

            data.Sampler = builder.Read(convertGBufferPassData.Sampler);

            SamplerDescriptor noiseSamplerDesc;
            noiseSamplerDesc.Renderer = this;
            noiseSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
            noiseSamplerDesc.AddressModeU = noiseSamplerDesc.AddressModeV = noiseSamplerDesc.AddressModeW = D3D11_TEXTURE_ADDRESS_WRAP;
            noiseSamplerDesc.CompFunc = D3D11_COMPARISON_ALWAYS;
            data.NoiseSampler = builder.Create<SamplerResource>("NoiseSampler", noiseSamplerDesc);

            data.Viewport = builder.Read(lightingPassData.Viewport);
            data.DepthDisableState = builder.Read(lightingPassData.DepthDisableState);

            data.CamRef = builder.Read(lightingPassData.CamRef);

            data.SSAOBaseDataBuffer = builder.Read(ssaoBaseDataBufferRefRes);

            ConstantBufferDescriptor ssaoParamsBufferDesc;
            ssaoParamsBufferDesc.Renderer = this;
            ssaoParamsBufferDesc.Size = sizeof(SSAOParamsConstantBuffer);
            data.SSAOParamsBuffer = builder.Create<ConstantBufferResource>(
               "SSAOParamsConstantBuffer",
               ssaoParamsBufferDesc);

            data.NoiseTextureRef = builder.Read(ssaoNoiseTexRefRes);
            data.GBufferRef = builder.Read(convertGBufferPassData.SourceGBufferRef);
            data.ViewspaceGBuffer = builder.Read(convertGBufferPassData.ConvertedGBuffer);

            data.QuadMeshRef = builder.Read(lightingPassData.QuadMeshRef);

            data.OutputRef = builder.Write(ssaoOutputRefRes);
         },
         [](const SSAOPassData& data)
         {
            OPTICK_EVENT("ExecuteSSAOPass");
            auto& profiler = data.Renderer->GetProfiler();
            ScopedGPUProfile profile(profiler, "SSAOPass");
            bool bSSAOEnabled = *(*data.SSAOEnabledRef->GetActual());
            if (bSSAOEnabled)
            {
               ID3D11DeviceContext& immediateContext = data.Renderer->GetImmediateContext();
               immediateContext.ClearState();
               immediateContext.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

               auto vertexShader = data.VertexShader->GetActual();
               auto pixelShader = data.PixelShader->GetActual();
               auto sampler = data.Sampler->GetActual();
               auto noiseSampler = data.NoiseSampler->GetActual();
               auto viewport = data.Viewport->GetActual();
               auto depthDisableState = data.DepthDisableState->GetActual();
               auto camera = *data.CamRef->GetActual();
               auto ssaoBaseDataBuffer = *data.SSAOBaseDataBuffer->GetActual();
               auto ssaoParamsBuffer = data.SSAOParamsBuffer->GetActual();
               auto noiseTexture = *data.NoiseTextureRef->GetActual();
               auto viewspaceGBuffer = data.ViewspaceGBuffer->GetActual();
               auto quadMesh = *data.QuadMeshRef->GetActual();
               auto output = *data.OutputRef->GetActual();
               auto depthBuffer = (*data.GBufferRef->GetActual())->GetDepthStencilBufferDX11();

               /** Binds */
               vertexShader->Bind(immediateContext);
               pixelShader->Bind(immediateContext);
               sampler->Bind(immediateContext, 0);
               noiseSampler->Bind(immediateContext, 1);
               viewport->Bind(immediateContext);
               depthDisableState->Bind(immediateContext);
               ssaoBaseDataBuffer->Bind(immediateContext, 0, EShaderType::PixelShader);
               ssaoParamsBuffer->Bind(immediateContext, 1, EShaderType::PixelShader);
               viewspaceGBuffer->BindAsShaderResource(immediateContext, 0, EShaderType::PixelShader);
               noiseTexture->Bind(immediateContext, 5, EShaderType::PixelShader);
               depthBuffer->BindAsShaderResource(immediateContext, 6, EShaderType::PixelShader);
               quadMesh->Bind(immediateContext, 0);
               output->Clear(immediateContext, Vector4::Zero());
               output->BindAsRenderTarget(immediateContext);

               /** Render */
               auto camTransform = camera->GetTransform();
               Matrix projMatrix = Matrix::CreatePerspectiveProj(
                  camera->GetFov(),
                  (output->GetWidth() / (float)output->GetHeight()),
                  camera->GetNearPlane(),
                  camera->GetFarPlane());

               const auto& ssaoParams = ((RendererPBR*)data.Renderer)->GetSSAOParams();

               auto mappedSSAOParamsBuffer = ssaoParamsBuffer->Map<SSAOParamsConstantBuffer>(immediateContext);
               ZeroMemory(mappedSSAOParamsBuffer, sizeof(SSAOParamsConstantBuffer));
               (*mappedSSAOParamsBuffer) = SSAOParamsConstantBuffer{ projMatrix, Vector3(ssaoParams.Radius, ssaoParams.Bias, ssaoParams.Magnitude) };
               ssaoParamsBuffer->UnMap(immediateContext);
               data.Renderer->DrawIndexed(quadMesh->GetVertexCount(), quadMesh->GetIndexCount());

               /** Unbinds */
               output->UnbindRenderTarget(immediateContext);
               viewspaceGBuffer->UnbindShaderResource(immediateContext, 0, EShaderType::PixelShader);
               depthBuffer->UnbindShaderResource(immediateContext, 6, EShaderType::PixelShader);
               noiseTexture->Unbind(immediateContext, 5, EShaderType::PixelShader);
               ssaoParamsBuffer->Unbind(immediateContext, 1, EShaderType::PixelShader);
               ssaoBaseDataBuffer->Unbind(immediateContext, 0, EShaderType::PixelShader);
               noiseSampler->Unbind(immediateContext, 1);
               sampler->Unbind(immediateContext, 0);
               pixelShader->Unbind(immediateContext);
               vertexShader->Unbind(immediateContext);
            }
         });

      const auto& ssaoPassData = ssaoPass->GetData();

      /** SSAO Blur */
      struct SSAOBlurPassData : public RenderPassDataBase
      {
         BoolRefResource* SSAOEnabledRef = nullptr;

         SamplerResource* Sampler = nullptr;

         RenderTargetRefResource* SourceRef = nullptr;
         ViewportResource* Viewport = nullptr;
         DepthStencilStateResource* DepthDisableState = nullptr;

         MeshRefResource* QuadMeshRef = nullptr;
         RenderTargetRefResource* OutputRef = nullptr;
      };

      auto ssaoBlurPassVSRes = m_frameGraph.AddExternalPermanentResource("SSAOBlurPassVertexShader", ShaderDescriptor(), m_ssaoBlurPassVS);
      auto ssaoBlurPassPSRes = m_frameGraph.AddExternalPermanentResource("SSAOBlurPassPixelShader", ShaderDescriptor(), m_ssaoBlurPassPS);

      auto blurredSSAORefRes = m_frameGraph.AddExternalPermanentResource("BlurredSSAO", RenderTargetRefDescriptor(), &m_blurredSSAO);

      auto ssaoBlurPass = m_frameGraph.AddCallbackPass<SSAOBlurPassData>(
         "SSAOBlurPass",
         [&](Elaina::RenderPassBuilder& builder, SSAOBlurPassData& data)
         {
            data.Renderer = this;
            data.VertexShader = builder.Read(ssaoBlurPassVSRes);
            data.PixelShader = builder.Read(ssaoBlurPassPSRes);

            data.SSAOEnabledRef = builder.Read(ssaoPassData.SSAOEnabledRef);

            data.Sampler = builder.Read(ssaoPassData.Sampler);

            data.SourceRef = builder.Read(ssaoPassData.OutputRef);
            data.Viewport = builder.Read(ssaoPassData.Viewport);
            data.DepthDisableState = builder.Read(ssaoPassData.DepthDisableState);

            data.QuadMeshRef = builder.Read(ssaoPassData.QuadMeshRef);

            RenderTargetDescriptor outputDesc;
            outputDesc.Renderer = this;
            outputDesc.ResolutionReference = &m_outputRenderTarget;
            outputDesc.Format = EColorFormat::R32_FLOAT;
            data.OutputRef = builder.Write(blurredSSAORefRes);
         },
         [](const SSAOBlurPassData& data)
         {
            OPTICK_EVENT("ExecuteSSAOBlurPass");
            bool bSSAOEnabled = *(*data.SSAOEnabledRef->GetActual());
            if (bSSAOEnabled)
            {
               auto& profiler = data.Renderer->GetProfiler();
               ScopedGPUProfile profile(profiler, "SSAOBlurPass");
               ID3D11DeviceContext& context = data.Renderer->GetImmediateContext();
               context.ClearState();
               context.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

               auto vertexShader = data.VertexShader->GetActual();
               auto pixelShader = data.PixelShader->GetActual();
               auto sampler = data.Sampler->GetActual();
               auto source = *data.SourceRef->GetActual();
               auto viewport = data.Viewport->GetActual();
               auto depthDisableState = data.DepthDisableState->GetActual();
               auto quadMesh = *data.QuadMeshRef->GetActual();
               auto output = *data.OutputRef->GetActual();

               /** Binds */
               vertexShader->Bind(context);
               pixelShader->Bind(context);
               sampler->Bind(context, 0);
               source->BindAsShaderResource(context, 0, EShaderType::PixelShader);
               viewport->Bind(context);
               depthDisableState->Bind(context);
               output->Clear(context, Vector4::Zero());
               output->BindAsRenderTarget(context);

               quadMesh->Bind(context, 0);

               /** Render */
               data.Renderer->DrawIndexed(quadMesh->GetVertexCount(), quadMesh->GetIndexCount());

               /** Unbinds */
               output->UnbindRenderTarget(context);
               source->UnbindShaderResource(context, 0, EShaderType::PixelShader);
               sampler->Unbind(context, 0);
               pixelShader->Unbind(context);
               vertexShader->Unbind(context);
            }
         });

      ssaoBlurPass->SetCullImmune(true);

      const auto& ssaoBlurPassData = ssaoBlurPass->GetData();

      /** Ambient-Emissive Pass */
      struct AmbientEmissivePassData : public RenderPassDataBase
      {
         ViewportResource* Viewport = nullptr;
         DepthStencilStateResource* DepthDisableState = nullptr;
         BlendStateResource* AdditiveBlendState = nullptr;
         SamplerResource* AnisoSampler = nullptr;
         SamplerResource* LinearClampSampler = nullptr;
         SamplerResource* SSAOSampler = nullptr;
         GBufferRefResource* GBufferRef = nullptr;
         DynamicCubemapRefResource* IrradianceMapRef = nullptr;
         DynamicCubemapRefResource* PrefilteredMapRef = nullptr;
         RenderTargetRefResource* BrdfLUTRef = nullptr;
         CameraRefResource* CamRef = nullptr;
         ConstantBufferResource* ParamsBuffer = nullptr;
         BoolRefResource* SSAOEnabledRef = nullptr;
         RenderTargetRefResource* BlurredSSAORef = nullptr;
         FloatRefResource* AmbientIntensityRef = nullptr;
         MeshRefResource* QuadMeshRef = nullptr;
         RenderTargetRefResource* OutputRef = nullptr;
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

            data.LinearClampSampler = builder.Read(convertSkyboxToCubemapPassData.Sampler);

            data.SSAOSampler = builder.Read(ssaoPassData.NoiseSampler);

            data.GBufferRef = builder.Read(geometryPassData.OutputGBufferRef);
            data.IrradianceMapRef = builder.Read(diffuseIntegralPassData.OutputIrradianceMapRef);
            data.PrefilteredMapRef = builder.Read(prefilterEnvMapPassData.OutputPrefilteredEnvMapRef);
            data.BrdfLUTRef = builder.Read(integrateBRDFPassData.OutputBrdfLUTRef);
            data.CamRef = builder.Read(ssaoPassData.CamRef);

            ConstantBufferDescriptor paramsBufferDesc;
            paramsBufferDesc.Renderer = this;
            paramsBufferDesc.Size = sizeof(AmbientParamsConstantBuffer);
            data.ParamsBuffer = builder.Create<ConstantBufferResource>("AmbientEmissiveParamsConstantBuffer", paramsBufferDesc);
            data.SSAOEnabledRef = builder.Read(ssaoBlurPassData.SSAOEnabledRef);
            data.BlurredSSAORef = builder.Read(ssaoBlurPassData.OutputRef);

            FloatRefDescriptor ambientIntensityDesc;
            ambientIntensityDesc.Reference = &m_ambientIntensity;
            data.AmbientIntensityRef = builder.Create<FloatRefResource>("AmbientIntensity", ambientIntensityDesc);

            data.QuadMeshRef = builder.Read(ssaoBlurPassData.QuadMeshRef);
            data.OutputRef = builder.Write(lightingPassData.OutputRef);
         },
         [](const AmbientEmissivePassData& data)
         {
            OPTICK_EVENT("ExecuteAmbientEmissivePass");
            auto& profiler = data.Renderer->GetProfiler();
            ScopedGPUProfile profile(profiler, "AmbientEmissivePass");
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
            auto gBuffer = *data.GBufferRef->GetActual();
            auto irraidianceMap = *data.IrradianceMapRef->GetActual();
            auto prefilteredMap = *data.PrefilteredMapRef->GetActual();
            auto brdfLUT = *data.BrdfLUTRef->GetActual();
            auto camera = *data.CamRef->GetActual();
            bool bSSAOEnabled = *(*data.SSAOEnabledRef->GetActual());
            auto blurredSSAO = *data.BlurredSSAORef->GetActual();
            auto quadMesh = *data.QuadMeshRef->GetActual();
            auto output = *data.OutputRef->GetActual();

            /** Binds */
            vertexShader->Bind(context);
            pixelShader->Bind(context);
            viewport->Bind(context);
            depthDisableState->Bind(context);
            additiveBlendState->Bind(context);
            anisoSampler->Bind(context, 0);
            linearClampSampler->Bind(context, 1);
            ssaoSampler->Bind(context, 2);
            gBuffer->BindAsShaderResource(context, 0, EShaderType::PixelShader);
            irraidianceMap->Bind(context, 5, EShaderType::PixelShader);
            prefilteredMap->Bind(context, 6, EShaderType::PixelShader);
            brdfLUT->BindAsShaderResource(context, 7, EShaderType::PixelShader);
            if (bSSAOEnabled)
            {
               blurredSSAO->BindAsShaderResource(context, 8, EShaderType::PixelShader);
            }
            quadMesh->Bind(context, 0);
            output->BindAsRenderTarget(context);

            /** Update Constant Buffer */
            auto camTransform = camera->GetTransform();
            auto paramsBuffer = data.ParamsBuffer->GetActual();
            float ambientIntensity = *(*data.AmbientIntensityRef->GetActual());
            auto mappedParamsBuffer = paramsBuffer->Map<AmbientParamsConstantBuffer>(context);
            (*mappedParamsBuffer) = AmbientParamsConstantBuffer{ camTransform->GetPosition(ETransformSpace::World), ambientIntensity, static_cast<unsigned int>(bSSAOEnabled) };
            paramsBuffer->UnMap(context);
            paramsBuffer->Bind(context, 0, EShaderType::PixelShader);

            /** Render */
            data.Renderer->DrawIndexed(quadMesh->GetVertexCount(), quadMesh->GetIndexCount());

            /** Unbinds */
            output->UnbindRenderTarget(context);
            blurredSSAO->UnbindShaderResource(context, 8, EShaderType::PixelShader);
            brdfLUT->UnbindShaderResource(context, 7, EShaderType::PixelShader);
            prefilteredMap->Unbind(context, 6, EShaderType::PixelShader);
            irraidianceMap->Unbind(context, 5, EShaderType::PixelShader);
            gBuffer->UnbindShaderResource(context, 0, EShaderType::PixelShader);
            ssaoSampler->Unbind(context, 2);
            linearClampSampler->Unbind(context, 1);
            anisoSampler->Unbind(context, 0);
            pixelShader->Unbind(context);
            vertexShader->Unbind(context);
         });

      const auto& ambientEmissivePassData = ambientEmissivePass->GetData();

      struct SkyboxPassData : public RenderPassDataBase
      {
         ViewportResource* Viewport = nullptr;
         CameraRefResource* CamRef = nullptr;
         GBufferRefResource* GBufferRef = nullptr;
         VoidRefResource* SkyboxType = nullptr;
         DynamicCubemapRefResource* EnvironmentMapRef = nullptr;
         DynamicCubemapRefResource* IrradianceMapRef = nullptr;
         SamplerResource* Sampler = nullptr;
         MeshRefResource* CubeMeshRef = nullptr;
         DepthStencilStateResource* DepthLessEqualState = nullptr;
         RasterizerStateResource* NoCullingState = nullptr;
         ConstantBufferResource* TransformConstantBuffer = nullptr;
         RenderTargetRefResource* OutputRef = nullptr;
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
            data.GBufferRef = builder.Read(ambientEmissivePassData.GBufferRef);

            VoidRefDescriptor skyboxRefDesc;
            skyboxRefDesc.Reference = (void*)&m_renderSkyboxType;
            data.SkyboxType = builder.Create<VoidRefResource>("SkyboxType", skyboxRefDesc);
            data.Sampler = builder.Read(convertSkyboxToCubemapPassData.Sampler);

            data.DepthLessEqualState = builder.Read(convertSkyboxToCubemapPassData.DepthLessEqualState);
            data.NoCullingState = builder.Read(convertSkyboxToCubemapPassData.NoCullingState);

            data.TransformConstantBuffer = builder.Write(convertGBufferPassData.ConvertParamsBuffer);

            data.EnvironmentMapRef = builder.Read(convertSkyboxToCubemapPassData.OutputEnvMapRef);
            data.IrradianceMapRef = builder.Read(diffuseIntegralPassData.OutputIrradianceMapRef);

            data.CubeMeshRef = builder.Read(convertSkyboxToCubemapPassData.CubeMeshRef);

            data.OutputRef = builder.Write(ambientEmissivePassData.OutputRef);
         },
         [](const SkyboxPassData& data)
         {
            OPTICK_EVENT("ExecuteSkyboxPass");
            auto& profiler = data.Renderer->GetProfiler();
            ID3D11DeviceContext& context = data.Renderer->GetImmediateContext();
            ScopedGPUProfile profile(profiler, "SkyboxPass");
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
            auto output = *data.OutputRef->GetActual();
            auto gBuffer = *data.GBufferRef->GetActual();
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
            output->BindAsRenderTarget(context);

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
            Matrix viewMat = Matrix::CreateView(Vector3(0.0f, 0.0f, 0.0f), camTransform->GetForward(ETransformSpace::World), camTransform->GetUp(ETransformSpace::World));
            Matrix projMat = Matrix::CreatePerspectiveProj(camera->GetFov(), output->GetAspectRatio(), 0.1f, 1000.0f);
            auto mappedTransformBuffer = transformBuffer->Map<OneMatrixConstantBuffer>(context);
            (*mappedTransformBuffer) = OneMatrixConstantBuffer{ viewMat * projMat };
            transformBuffer->UnMap(context);

            /** Render */
            data.Renderer->DrawIndexed(cubeMesh->GetVertexCount(), cubeMesh->GetIndexCount());

            /** Unbinds */
            switch (skyboxType)
            {
            case ESkyboxType::IrradianceMap:
               irrdianceMap->Unbind(context, 0, EShaderType::PixelShader);
               break;
            case ESkyboxType::EnvironmentMap:
            default:
               envMap->Unbind(context, 0, EShaderType::PixelShader);
               break;
            }

            output->UnbindRenderTarget(context);
            output->SetDepthStencilBuffer(nullptr);
            transformBuffer->Unbind(context, 0, EShaderType::VertexShader);
            sampler->Unbind(context, 0);
            pixelShader->Unbind(context);
            vertexShader->Unbind(context);
         });

      const auto& skyboxPassData = skyboxPass->GetData();

      /** Extract Brightness Pass */
      struct ExtractBrightnessPassData : public RenderPassDataBase
      {
         SamplerResource* Sampler = nullptr;
         ViewportResource* Viewport = nullptr;
         RenderTargetRefResource* InputRef = nullptr;
         VoidRefResource* BloomParamsRef = nullptr;
         ConstantBufferResource* ParamsBuffer = nullptr;
         DepthStencilStateResource* DepthDisableState = nullptr;
         MeshRefResource* QuadMeshRef = nullptr;
         RenderTargetRefResource* OutputRef = nullptr;
      };

      auto extractBrightnessPassVSRes = m_frameGraph.AddExternalPermanentResource("ExtractBrightnessPassVertexShader", ShaderDescriptor(), m_extractBrightnessPassVS);
      auto extractBrightnessPassPSRes = m_frameGraph.AddExternalPermanentResource("ExtractBrightnessPassPixelShader", ShaderDescriptor(), m_extractBrightnessPassPS);

      auto extractedBrightnessRefRes = m_frameGraph.AddExternalPermanentResource("ExtracterBrightnessRef", RenderTargetRefDescriptor(), &m_extractedBrightness);
      auto extractBrightnessPass = m_frameGraph.AddCallbackPass<ExtractBrightnessPassData>(
         "ExtractBrightnessPass",
         [&](Elaina::RenderPassBuilder& builder, ExtractBrightnessPassData& data)
         {
            data.Renderer = this;
            data.VertexShader = builder.Read(extractBrightnessPassVSRes);
            data.PixelShader = builder.Read(extractBrightnessPassPSRes);

            data.Sampler = builder.Read(convertSkyboxToCubemapPassData.Sampler);

            data.Viewport = builder.Read(skyboxPassData.Viewport);
            data.InputRef = builder.Read(skyboxPassData.OutputRef);

            VoidRefDescriptor bloomParamRefDesc;
            bloomParamRefDesc.Reference = &m_bloomParams;
            data.BloomParamsRef = builder.Create<VoidRefResource>("BloomParams", bloomParamRefDesc);

            data.ParamsBuffer = builder.Write(prefilterEnvMapPassData.PrefilterParamsBuffer);

            data.DepthDisableState = builder.Read(ambientEmissivePassData.DepthDisableState);
            data.QuadMeshRef = builder.Read(ambientEmissivePassData.QuadMeshRef);

            data.OutputRef = builder.Write(extractedBrightnessRefRes);
         },
         [](const ExtractBrightnessPassData& data)
         {
            OPTICK_EVENT("ExecuteExtractBrightnessPass");
            auto& profiler = data.Renderer->GetProfiler();
            ID3D11DeviceContext& context = data.Renderer->GetImmediateContext();
            ScopedGPUProfile profile(profiler, "ExtractBrightnessPass");
            context.ClearState();
            context.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            auto vertexShader = data.VertexShader->GetActual();
            auto pixelShader = data.PixelShader->GetActual();
            auto sampler = data.Sampler->GetActual();
            auto input = *data.InputRef->GetActual();
            auto bloomParams = (BloomParams*)(*data.BloomParamsRef->GetActual());
            auto paramsBuffer = data.ParamsBuffer->GetActual();
            auto depthDisableState = data.DepthDisableState->GetActual();
            auto quadMesh = *data.QuadMeshRef->GetActual();
            auto viewport = data.Viewport->GetActual();
            auto output = *data.OutputRef->GetActual();

            /** Binds */
            vertexShader->Bind(context);
            pixelShader->Bind(context);
            sampler->Bind(context, 0);
            input->BindAsShaderResource(context, 0, EShaderType::PixelShader);
            paramsBuffer->Bind(context, 0, EShaderType::PixelShader);
            depthDisableState->Bind(context);
            quadMesh->Bind(context, 0);
            viewport->Bind(context);
            output->Clear(context, Vector4(0.0f, 0.0f, 0.0f, 0.0f));
            output->BindAsRenderTarget(context);

            /** Update Constant Buffer */
            auto mappedParamsBuffer = paramsBuffer->Map<OneFloatConstantBuffer>(context);
            (*mappedParamsBuffer) = OneFloatConstantBuffer{ bloomParams->BrightnessThreshold };
            paramsBuffer->UnMap(context);

            /** Render */
            data.Renderer->DrawIndexed(quadMesh->GetVertexCount(), quadMesh->GetIndexCount());

            /** Unbinds */
            output->UnbindRenderTarget(context);
            paramsBuffer->Unbind(context, 0, EShaderType::PixelShader);
            input->UnbindShaderResource(context, 0, EShaderType::PixelShader);
            sampler->Unbind(context, 0);
            pixelShader->Unbind(context);
            vertexShader->Unbind(context);
         });

      const auto& extractBrightnessPassData = extractBrightnessPass->GetData();

      /** Bloom - Gaussian Blur Pass */
      struct BloomGaussBlurPassData : public RenderPassDataBase
      {
         VoidRefResource* BloomParamsRef = nullptr;
         SamplerResource* Sampler = nullptr;
         ViewportResource* Viewport = nullptr;
         DepthStencilStateResource* DepthDisableState = nullptr;
         ConstantBufferResource* ParamsBuffer = nullptr;
         MeshRefResource* QuadMeshRef = nullptr;
         RenderTargetRefResource* InputRef = nullptr;
         std::array<RenderTargetRefResource*, 2> PingPongBufferRef{ nullptr, }; /** Latest output = (BlurAmount)%2 */
      };

      auto gaussianBlurVSRes = m_frameGraph.AddExternalPermanentResource(
         "GaussianBlurVertexShader",
         ShaderDescriptor(),
         m_gaussBloomPassVS);

      auto gaussianBlurPSRes = m_frameGraph.AddExternalPermanentResource(
         "GaussianBlurPixelShader",
         ShaderDescriptor(),
         m_gaussBloomPassPS);

      std::array<RenderTargetRefResource*, 2> pingPongBuffersRefRes{ nullptr, };
      for (size_t idx = 0; idx < m_pingPongBuffers.size(); ++idx)
      {
         pingPongBuffersRefRes[idx] =
            m_frameGraph.AddExternalPermanentResource(
               "PingPongBufferRef_" + std::to_string(idx),
               RenderTargetRefDescriptor(), &m_pingPongBuffers[idx]);
      }

      auto bloomGaussBlurPass = m_frameGraph.AddCallbackPass<BloomGaussBlurPassData>(
         "BloomGaussBlurPass",
         [&](Elaina::RenderPassBuilder& builder, BloomGaussBlurPassData& data)
         {
            data.Renderer = this;
            data.VertexShader = builder.Read(gaussianBlurVSRes);
            data.PixelShader = builder.Read(gaussianBlurPSRes);

            data.BloomParamsRef = builder.Read(extractBrightnessPassData.BloomParamsRef);
            data.Sampler = builder.Read(extractBrightnessPassData.Sampler);
            data.Viewport = builder.Read(extractBrightnessPassData.Viewport);
            data.DepthDisableState = builder.Read(extractBrightnessPassData.DepthDisableState);

            data.QuadMeshRef = builder.Read(extractBrightnessPassData.QuadMeshRef);
            data.InputRef = builder.Read(extractBrightnessPassData.OutputRef);

            ConstantBufferDescriptor paramsBufferDesc;
            paramsBufferDesc.Renderer = this;
            paramsBufferDesc.Size = sizeof(OneUINTConstantBuffer);
            data.ParamsBuffer = builder.Create<ConstantBufferResource>("GaussianBlurParamsConstantBuffer", paramsBufferDesc);

            for (size_t idx = 0; idx < data.PingPongBufferRef.size(); ++idx)
            {
               data.PingPongBufferRef[idx] = builder.Write(pingPongBuffersRefRes[idx]);
            }
         },
         [](const BloomGaussBlurPassData& data)
         {
            OPTICK_EVENT("ExecuteBloomGaussBlurPass");
            auto& profiler = data.Renderer->GetProfiler();
            ID3D11DeviceContext& context = data.Renderer->GetImmediateContext();
            ScopedGPUProfile profile(profiler, "BloomGaussBlurPass");
            context.ClearState();
            context.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            auto vertexShader = data.VertexShader->GetActual();
            auto pixelShader = data.PixelShader->GetActual();
            auto sampler = data.Sampler->GetActual();
            auto bloomParams = (BloomParams*)(*data.BloomParamsRef->GetActual());
            auto viewport = data.Viewport->GetActual();
            auto depthDisableState = data.DepthDisableState->GetActual();
            auto quadMesh = *data.QuadMeshRef->GetActual();
            auto paramsBuffer = data.ParamsBuffer->GetActual();
            auto input = *data.InputRef->GetActual();
            std::array<RenderTargetDX11*, 2> pingPongBuffer{ *data.PingPongBufferRef[0]->GetActual(), *data.PingPongBufferRef[1]->GetActual() };

            /** Binds */
            vertexShader->Bind(context);
            pixelShader->Bind(context);
            sampler->Bind(context, 0);
            viewport->Bind(context);
            depthDisableState->Bind(context);
            quadMesh->Bind(context, 0);
            paramsBuffer->Bind(context, 0, EShaderType::PixelShader);

            /** Render */
            auto latestInput = input;
            auto latestOutput = pingPongBuffer[0];
            bool bHorizontal = true;
            for (unsigned int epoch = 0; epoch < bloomParams->BlurAmount; ++epoch)
            {
               if (epoch > 0)
               {
                  bHorizontal = !bHorizontal;
                  unsigned int swap = epoch % 2;
                  switch (swap)
                  {
                  case 0:
                     latestInput = pingPongBuffer[1];
                     latestOutput = pingPongBuffer[0];
                     break;

                  case 1:
                     latestInput = pingPongBuffer[0];
                     latestOutput = pingPongBuffer[1];
                     break;
                  }
               }

               latestInput->BindAsShaderResource(context, 0, EShaderType::PixelShader);
               latestOutput->Clear(context, Vector4(0.0f, 0.0f, 0.0f, 0.0f));
               latestOutput->BindAsRenderTarget(context);

               auto mappedParamsBuffer = paramsBuffer->Map<OneUINTConstantBuffer>(context);
               (*mappedParamsBuffer) = OneUINTConstantBuffer{ static_cast<unsigned int>(bHorizontal ? 1 : 0) };
               paramsBuffer->UnMap(context);

               data.Renderer->DrawIndexed(quadMesh->GetVertexCount(), quadMesh->GetIndexCount());

               latestOutput->UnbindRenderTarget(context);
               latestInput->UnbindShaderResource(context, 0, EShaderType::PixelShader);
            }

            /** Unbinds */
            paramsBuffer->Unbind(context, 0, EShaderType::PixelShader);
            sampler->Unbind(context, 0);
            pixelShader->Unbind(context);
            vertexShader->Unbind(context);
         });

      const auto& bloomGaussBlurPassData = bloomGaussBlurPass->GetData();

      /** Bloom - Blend */
      struct BloomBlendPassData : public RenderPassDataBase
      {
         VoidRefResource* BloomParamsRef = nullptr;
         SamplerResource* Sampler = nullptr;
         ViewportResource* Viewport = nullptr;
         DepthStencilStateResource* DepthDisableState = nullptr;
         BlendStateResource* AdditiveBlendState = nullptr;
         MeshRefResource* QuadMeshRef = nullptr;

         ConstantBufferResource* ParamsBuffer = nullptr;
         std::array<RenderTargetRefResource*, 2> PingPongBufferRef{ nullptr, };
         RenderTargetRefResource* OutputRef = nullptr;
      };

      auto bloomBlendPass = m_frameGraph.AddCallbackPass<BloomBlendPassData>(
         "BloomBlendPass",
         [&](Elaina::RenderPassBuilder& builder, BloomBlendPassData& data)
         {
            data.Renderer = this;
            data.VertexShader = builder.Read(printTextureVSRes);
            data.PixelShader = builder.Read(printTexturePSRes);

            data.Sampler = builder.Read(bloomGaussBlurPassData.Sampler);

            data.BloomParamsRef = builder.Read(bloomGaussBlurPassData.BloomParamsRef);
            data.Viewport = builder.Read(bloomGaussBlurPassData.Viewport);
            data.DepthDisableState = builder.Read(bloomGaussBlurPassData.DepthDisableState);
            data.AdditiveBlendState = builder.Read(lightingPassData.AdditiveBlendState);

            data.ParamsBuffer = builder.Write(prefilterEnvMapPassData.PrefilterParamsBuffer);

            data.QuadMeshRef = builder.Read(bloomGaussBlurPassData.QuadMeshRef);
            data.PingPongBufferRef[0] = builder.Read(bloomGaussBlurPassData.PingPongBufferRef[0]);
            data.PingPongBufferRef[1] = builder.Read(bloomGaussBlurPassData.PingPongBufferRef[1]);

            data.OutputRef = builder.Write(skyboxPassData.OutputRef);
         },
         [](const BloomBlendPassData& data)
         {
            OPTICK_EVENT("ExecuteBloomBlendPass");
            auto& profiler = data.Renderer->GetProfiler();
            ID3D11DeviceContext& context = data.Renderer->GetImmediateContext();
            ScopedGPUProfile profile(profiler, "BloomBlendPass");
            context.ClearState();
            context.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            auto vertexShader = data.VertexShader->GetActual();
            auto pixelShader = data.PixelShader->GetActual();
            auto sampler = data.Sampler->GetActual();
            auto bloomParams = (BloomParams*)(*data.BloomParamsRef->GetActual());
            auto viewport = data.Viewport->GetActual();
            auto depthDisableState = data.DepthDisableState->GetActual();
            auto additiveBlendState = data.AdditiveBlendState->GetActual();
            auto paramsBuffer = data.ParamsBuffer->GetActual();
            auto quadMesh = *data.QuadMeshRef->GetActual();
            auto input = *data.PingPongBufferRef[bloomParams->BlurAmount % 2]->GetActual();
            auto output = *data.OutputRef->GetActual();

            /** Binds */
            vertexShader->Bind(context);
            pixelShader->Bind(context);
            sampler->Bind(context, 0);
            viewport->Bind(context);
            depthDisableState->Bind(context);
            additiveBlendState->Bind(context);
            paramsBuffer->Bind(context, 0, EShaderType::PixelShader);
            quadMesh->Bind(context, 0);
            input->BindAsShaderResource(context, 0, EShaderType::PixelShader);
            output->BindAsRenderTarget(context);

            /** Update Constant Buffers */
            auto mappedParamsBuffer = paramsBuffer->Map<OneFloatConstantBuffer>(context);
            (*mappedParamsBuffer) = OneFloatConstantBuffer{ bloomParams->Intensity };
            paramsBuffer->UnMap(context);

            /** Render */
            data.Renderer->DrawIndexed(quadMesh->GetVertexCount(), quadMesh->GetIndexCount());

            /** Unbinds */
            output->UnbindRenderTarget(context);
            input->UnbindShaderResource(context, 0, EShaderType::PixelShader);
            paramsBuffer->Unbind(context, 0, EShaderType::PixelShader);
            sampler->Unbind(context, 0);
            pixelShader->Unbind(context);
            vertexShader->Unbind(context);
         });

      const auto& bloomBlendPassData = bloomBlendPass->GetData();

      /** Tone Mapping Pass */
      struct ToneMappingPassData : public RenderPassDataBase
      {
         SamplerResource* Sampler = nullptr;
         ViewportResource* Viewport = nullptr;
         DepthStencilStateResource* DepthDisableState = nullptr;
         VoidRefResource* Params = nullptr;
         ConstantBufferResource* ParamsBuffer = nullptr;
         MeshRefResource* QuadMeshRef = nullptr;
         RenderTargetRefResource* InputRef = nullptr;
         RenderTargetRefResource* OutputRef = nullptr;
      };

      auto toneMappingVSRes = m_frameGraph.AddExternalPermanentResource("ToneMappingVertexShader", ShaderDescriptor(), m_toneMappingVS);
      auto toneMappingPSRes = m_frameGraph.AddExternalPermanentResource("ToneMappingPixelShader", ShaderDescriptor(), m_toneMappingPS);

      auto toneMappingPass = m_frameGraph.AddCallbackPass<ToneMappingPassData>("ToneMappingPass",
         [&](Elaina::RenderPassBuilder& builder, ToneMappingPassData& data)
         {
            data.Renderer = this;
            data.VertexShader = builder.Read(toneMappingVSRes);
            data.PixelShader = builder.Read(toneMappingPSRes);
            data.Sampler = builder.Read(bloomBlendPassData.Sampler);
            data.Viewport = builder.Read(bloomBlendPassData.Viewport);
            data.DepthDisableState = builder.Read(bloomBlendPassData.DepthDisableState);

            VoidRefDescriptor paramsDesc;
            paramsDesc.Reference = &m_toneMappingParams;
            data.Params = builder.Create<VoidRefResource>("ToneMappingParams", paramsDesc);

            ConstantBufferDescriptor toneMappingBufferDesc;
            toneMappingBufferDesc.Renderer = this;
            toneMappingBufferDesc.Size = sizeof(OneVector2ConstantBuffer);
            data.ParamsBuffer = builder.Create<ConstantBufferResource>("ToneMappingConstantBuffer", toneMappingBufferDesc);
            data.QuadMeshRef = builder.Read(bloomBlendPassData.QuadMeshRef);
            data.InputRef = builder.Read(bloomBlendPassData.OutputRef);
            data.OutputRef = builder.Write(outputRenderTargetRefRes);
         },
         [](const ToneMappingPassData& data)
         {
            OPTICK_EVENT("ExecuteToneMappingPass");
            auto& profiler = data.Renderer->GetProfiler();
            ID3D11DeviceContext& context = data.Renderer->GetImmediateContext();
            ScopedGPUProfile profile(profiler, "ToneMappingPass");
            context.ClearState();
            context.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            auto vertexShader = data.VertexShader->GetActual();
            auto pixelShader = data.PixelShader->GetActual();
            auto sampler = data.Sampler->GetActual();
            auto viewport = data.Viewport->GetActual();
            auto depthDisableState = data.DepthDisableState->GetActual();
            auto paramsBuffer = data.ParamsBuffer->GetActual();
            auto params = (ToneMappingParams*)(*data.Params->GetActual());
            auto quadMesh = *data.QuadMeshRef->GetActual();
            auto input = *data.InputRef->GetActual();
            auto output = *data.OutputRef->GetActual();

            /** Binds */
            vertexShader->Bind(context);
            pixelShader->Bind(context);
            sampler->Bind(context, 0);
            viewport->Bind(context);
            depthDisableState->Bind(context);
            paramsBuffer->Bind(context, 0, EShaderType::PixelShader);
            quadMesh->Bind(context, 0);
            input->BindAsShaderResource(context, 0, EShaderType::PixelShader);
            output->BindAsRenderTarget(context);

            /** Update Constant Buffers */
            auto mappedParamsBuffer = paramsBuffer->Map<OneVector2ConstantBuffer>(context);
            (*mappedParamsBuffer) = OneVector2ConstantBuffer{ Vector2(params->ExposureFactor, params->GammaFactor) };
            paramsBuffer->UnMap(context);

            /** Render */
            data.Renderer->DrawIndexed(quadMesh->GetVertexCount(), quadMesh->GetIndexCount());

            /** Unbinds */
            output->UnbindRenderTarget(context);
            input->UnbindShaderResource(context, 0, EShaderType::PixelShader);
            paramsBuffer->Unbind(context, 0, EShaderType::PixelShader);
            sampler->Unbind(context, 0);
            pixelShader->Unbind(context);
            vertexShader->Unbind(context);
         });

      struct DebugDepthSSAOPassData : public RenderPassDataBase
      {
         SamplerResource* Sampler = nullptr;
         ViewportResource* Viewport = nullptr;
         DepthStencilStateResource* DepthDisableState = nullptr;
         GBufferRefResource* InputGBufferRef = nullptr;
         RenderTargetRefResource* InputSSAORef = nullptr;
         MeshRefResource* QuadMeshRef = nullptr;
         CameraRefResource* CamRef = nullptr;
         ConstantBufferResource* TypeBuffer = nullptr;
         RenderTargetRefResource* OutputDepthRef = nullptr;
         RenderTargetRefResource* OutputSSAORef = nullptr;
      };

      auto debugDepthSSAOVSRes = m_frameGraph.AddExternalPermanentResource("DebugDepthSSAOVertexShader", ShaderDescriptor(), m_debugDepthSSAOVS);
      auto debugDepthSSAOPSRes = m_frameGraph.AddExternalPermanentResource("DebugDepthSSAOPixelShader", ShaderDescriptor(), m_debugDepthSSAOPS);
      auto debugDepthOutputRefRes = m_frameGraph.AddExternalPermanentResource("DebugDepthRef", RenderTargetRefDescriptor(), &m_depthDebugBuffer);
      auto debugSSAOOutputRefRes = m_frameGraph.AddExternalPermanentResource("DebugDepthRef", RenderTargetRefDescriptor(), &m_ssaoDebugBuffer);

      auto debugDepthSSAOPass = m_frameGraph.AddCallbackPass<DebugDepthSSAOPassData>("DebugDepthSSAOPass",
         [&](Elaina::RenderPassBuilder& builder, DebugDepthSSAOPassData& data)
         {
            data.Renderer = this;
            data.VertexShader = builder.Read(debugDepthSSAOVSRes);
            data.PixelShader = builder.Read(debugDepthSSAOPSRes);
            data.Sampler = builder.Read(bloomBlendPassData.Sampler);
            data.Viewport = builder.Read(bloomBlendPassData.Viewport);
            data.DepthDisableState = builder.Read(bloomBlendPassData.DepthDisableState);
            data.QuadMeshRef = builder.Read(bloomBlendPassData.QuadMeshRef);
            data.InputGBufferRef = builder.Read(geometryPassData.OutputGBufferRef);
            data.InputSSAORef = builder.Read(blurredSSAORefRes);
            data.OutputDepthRef = builder.Write(debugDepthOutputRefRes);
            data.OutputSSAORef = builder.Write(debugSSAOOutputRefRes);

            data.CamRef = builder.Read(geometryPassData.TargetCameraRef);

            ConstantBufferDescriptor bufferDesc;
            bufferDesc.Renderer = this;
            bufferDesc.Size = sizeof(DebugDepthSSAOConstantBuffer);
            data.TypeBuffer = builder.Create<ConstantBufferResource>("DebugTypeBuffer", bufferDesc);
         },
         [](const DebugDepthSSAOPassData& data)
         {
            OPTICK_EVENT("ExecuteDebugDepthSSAO");
            auto& profiler = data.Renderer->GetProfiler();
            ID3D11DeviceContext& context = data.Renderer->GetImmediateContext();
            ScopedGPUProfile profile(profiler, "DebugDepthSSAOPass");
            context.ClearState();
            context.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            auto vertexShader = data.VertexShader->GetActual();
            auto pixelShader = data.PixelShader->GetActual();
            auto sampler = data.Sampler->GetActual();
            auto viewport = data.Viewport->GetActual();
            auto depthDisableState = data.DepthDisableState->GetActual();
            auto quadMesh = *data.QuadMeshRef->GetActual();
            auto debugTypeBuffer = data.TypeBuffer->GetActual();
            auto camera = *data.CamRef->GetActual();

            auto gBuffer = *data.InputGBufferRef->GetActual();
            auto ssao = *data.InputSSAORef->GetActual();
            auto outputDepth = *data.OutputDepthRef->GetActual();
            auto outputSSAO = *data.OutputSSAORef->GetActual();

            /** Binds */
            vertexShader->Bind(context);
            pixelShader->Bind(context);
            sampler->Bind(context, 0);
            viewport->Bind(context);
            depthDisableState->Bind(context);
            quadMesh->Bind(context, 0);
            debugTypeBuffer->Bind(context, 0, EShaderType::PixelShader);

            /** Render */
            auto depthStencilBuffer = gBuffer->GetDepthStencilBufferDX11();

            DebugDepthSSAOConstantBuffer* mappedBuffer = debugTypeBuffer->Map<DebugDepthSSAOConstantBuffer>(context);
            (*mappedBuffer) = DebugDepthSSAOConstantBuffer{ 0, Vector2(camera->GetNearPlane(), camera->GetFarPlane()) };
            debugTypeBuffer->UnMap(context);

            outputDepth->BindAsRenderTarget(context);
            depthStencilBuffer->BindAsShaderResource(context, 0, EShaderType::PixelShader);
            data.Renderer->DrawIndexed(quadMesh->GetVertexCount(), quadMesh->GetIndexCount());
            depthStencilBuffer->UnbindShaderResource(context, 0, EShaderType::PixelShader);
            outputDepth->UnbindRenderTarget(context);

            mappedBuffer = debugTypeBuffer->Map<DebugDepthSSAOConstantBuffer>(context);
            (*mappedBuffer) = DebugDepthSSAOConstantBuffer{ 1 };
            debugTypeBuffer->UnMap(context);

            outputSSAO->BindAsRenderTarget(context);
            ssao->BindAsShaderResource(context, 0, EShaderType::PixelShader);
            data.Renderer->DrawIndexed(quadMesh->GetVertexCount(), quadMesh->GetIndexCount());
            ssao->UnbindShaderResource(context, 0, EShaderType::PixelShader);
            outputSSAO->UnbindRenderTarget(context);

            /** Unbinds */
            debugTypeBuffer->Unbind(context, 0, EShaderType::PixelShader);
            sampler->Unbind(context, 0);
            pixelShader->Unbind(context);
            vertexShader->Unbind(context);
         });

      m_frameGraph.Compile();

      Elaina::VisualizeParams visualizeParams;
      visualizeParams.bSplines = true;
      visualizeParams.RankSep = 3.0;
      m_frameGraph.ExportVisualization("RendererPBR.dot", visualizeParams);
      return true;
   }

   void RendererPBR::SetupSSAOParams()
   {
      OPTICK_EVENT();
      Vector2 renderRes = GetRenderResolution();
      unsigned int width = (unsigned int)renderRes.x;
      unsigned int height = (unsigned int)renderRes.y;
      m_ssaoBaseData.NoiseScale = Vector2(width / (float)RendererPBRConstants::SSAONoiseTextureSize, height / (float)RendererPBRConstants::SSAONoiseTextureSize);

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

         m_ssaoBaseData.Samples[idx] = sample;
      }

      if (m_ssaoBaseDataBuffer == nullptr)
      {
         ConstantBufferDescriptor baseDataBufferDesc;
         baseDataBufferDesc.Renderer = this;
         baseDataBufferDesc.Size = sizeof(SSAOBaseData);
         m_ssaoBaseDataBuffer = Elaina::Realize<ConstantBufferDescriptor, ConstantBufferDX11>(baseDataBufferDesc);
      }

      SSAOBaseData* mappedBuffer = m_ssaoBaseDataBuffer->Map<SSAOBaseData>(GetImmediateContext());
      (*mappedBuffer) = m_ssaoBaseData;
      m_ssaoBaseDataBuffer->UnMap(GetImmediateContext());

      if (m_ssaoNoiseTex == nullptr)
      {
         size_t noiseTexRes2 = ((size_t)RendererPBRConstants::SSAONoiseTextureSize * (size_t)RendererPBRConstants::SSAONoiseTextureSize);
         for (size_t idx = 0; idx < noiseTexRes2; ++idx)
         {
            m_ssaoParams.Noise[idx] = Vector4(
               randomFloats(generator) * 2.0f - 1.0f,
               randomFloats(generator) * 2.0f - 1.0f,
               0.0f,
               0.0f);
         }

         Texture2dDX11Descriptor noiseTexDesc;
         noiseTexDesc.Renderer = this;
         noiseTexDesc.Width = noiseTexDesc.Height = RendererPBRConstants::SSAONoiseTextureSize;
         noiseTexDesc.Channels = 4;
         noiseTexDesc.Format = static_cast<DXGI_FORMAT>(EColorFormat::R32G32B32A32_FLOAT);
         noiseTexDesc.Data = (unsigned char*)(m_ssaoParams.Noise.data());
         m_ssaoNoiseTex = Elaina::Realize<Texture2dDX11Descriptor, Texture2dDX11>(noiseTexDesc);
      }
   }

   void RendererPBR::RenderImpl(const World& world)
   {
      OPTICK_EVENT();
      AcquireRenderResources(world);

      m_targetCamera = nullptr;

      for (auto camera : m_cameras)
      {
         RenderTexture* renderTexture = camera->GetRenderTexture();
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

         m_outputRenderTarget->Clear(GetImmediateContext(), camera->GetClearColor());
         if (camera->IsActivated())
         {
            m_targetCamera = camera;
            m_frameGraph.Execute();
         }

         m_outputRenderTarget = nullptr;
      }
   }

   void RendererPBR::OnRenderResolutionChanged()
   {
      OPTICK_EVENT();
      SetupRenderResources();
      SetupSSAOParams();
   }

   void RendererPBR::SetupRenderResources()
   {
      OPTICK_EVENT();
      SafeDelete(m_gBuffer);
      SafeDelete(m_hdrBuffer);
      SafeDelete(m_extractedBrightness);
      SafeDelete(m_ssao);
      SafeDelete(m_blurredSSAO);
      SafeDelete(m_pingPongBuffers[0]);
      SafeDelete(m_pingPongBuffers[1]);
      SafeDelete(m_depthDebugBuffer);
      SafeDelete(m_ssaoDebugBuffer);
      SafeDelete(m_lightingDebugBuffer);

      auto renderRes = this->GetRenderResolution();
      GBufferDescriptor gBufferDesc;
      gBufferDesc.Renderer = this;
      gBufferDesc.Width = (unsigned int)renderRes.x;
      gBufferDesc.Height = (unsigned int)renderRes.y;
      m_gBuffer = Elaina::Realize<GBufferDescriptor, GBuffer>(gBufferDesc);

      RenderTargetDescriptor outputHDRBufferDesc;
      outputHDRBufferDesc.Renderer = this;
      outputHDRBufferDesc.Width = (unsigned int)renderRes.x;
      outputHDRBufferDesc.Height = (unsigned int)renderRes.y;
      outputHDRBufferDesc.Format = EColorFormat::R16G16B16A16_FLOAT;
      m_hdrBuffer = Elaina::Realize<RenderTargetDescriptor, RenderTargetDX11>(outputHDRBufferDesc);

      RenderTargetDescriptor brightnessRenderTargetDesc;
      brightnessRenderTargetDesc.Renderer = this;
      brightnessRenderTargetDesc.ResolutionReference = &m_hdrBuffer;
      brightnessRenderTargetDesc.FormatReference = &m_hdrBuffer;
      m_extractedBrightness = Elaina::Realize<RenderTargetDescriptor, RenderTargetDX11>(brightnessRenderTargetDesc);

      RenderTargetDescriptor pingPongBufferDesc;
      pingPongBufferDesc.Renderer = this;
      pingPongBufferDesc.ResolutionReference = &m_hdrBuffer;
      pingPongBufferDesc.FormatReference = &m_hdrBuffer;
      for (size_t idx = 0; idx < m_pingPongBuffers.size(); ++idx)
      {
         m_pingPongBuffers[idx] = Elaina::Realize<RenderTargetDescriptor, RenderTargetDX11>(pingPongBufferDesc);
      }

      RenderTargetDescriptor outputSSAODesc;
      outputSSAODesc.Renderer = this;
      outputSSAODesc.ResolutionReference = &m_hdrBuffer;
      outputSSAODesc.Format = EColorFormat::R32_FLOAT;
      m_ssao = Elaina::Realize<RenderTargetDescriptor, RenderTargetDX11>(outputSSAODesc);
      m_blurredSSAO = Elaina::Realize<RenderTargetDescriptor, RenderTargetDX11>(outputSSAODesc);

      RenderTargetDescriptor debugBufferDesc;
      debugBufferDesc.Renderer = this;
      debugBufferDesc.ResolutionReference = &m_hdrBuffer;
      debugBufferDesc.Format = EColorFormat::R8G8B8A8_UNORM;
      m_depthDebugBuffer = Elaina::Realize<RenderTargetDescriptor, RenderTargetDX11>(debugBufferDesc);
      m_ssaoDebugBuffer = Elaina::Realize<RenderTargetDescriptor, RenderTargetDX11>(debugBufferDesc);
      m_lightingDebugBuffer = Elaina::Realize<RenderTargetDescriptor, RenderTargetDX11>(debugBufferDesc);
   }

   void RendererPBR::AcquireRenderResources(const World& world)
   {
      OPTICK_EVENT();
      auto threadPool = Engine::GetThreadPool();
      auto acquireMeshRenderersAndMatTask = threadPool->AddTask([&]()
         {
            OPTICK_EVENT("AcquireMeshRenderersAndMaterial");
            for (auto& meshes : m_materialMap)
            {
               meshes.second.resize(0);
            }

            m_meshes.resize(0);
            world.GetComponentsFromEntities<MeshRenderComponent>(m_meshes);
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
            OPTICK_EVENT("AcquireLights");
            m_lights.resize(0);
            world.GetComponentsFromEntities<LightComponent>(m_lights);
         });
      auto acquireCamerasTask = threadPool->AddTask([&]()
         {
            OPTICK_EVENT("AcquireCameras");
            m_cameras.resize(0);
            world.GetComponentsFromEntities<CameraComponent>(m_cameras, false);
         });
      auto acquireSkyboxTask = threadPool->AddTask([&]()
         {
            OPTICK_EVENT("AcquireSkybox");
            m_skyboxTexture = nullptr;
            auto skyboxComponents{ std::move(world.GetComponentsFromEntities<SkyboxComponent>()) };
            if (skyboxComponents.size() > 0)
            {
               m_skyboxTexture = skyboxComponents[0]->GetTexture();
               if (m_skyboxTexture != m_oldSkyboxTexture)
               {
                  m_oldSkyboxTexture = m_skyboxTexture;
                  m_bPrecomputeIBL = true;
               }
            }
            else
            {
               m_skyboxTexture = nullptr;
               if (m_oldSkyboxTexture != nullptr)
               {
                  m_oldSkyboxTexture = nullptr;
                  m_bPrecomputeIBL = true;
               }
            }
         });

      acquireMeshRenderersAndMatTask.get();
      acquireLightsTask.get();
      acquireCamerasTask.get();
      acquireSkyboxTask.get();
   }

   void RendererPBR::RenderMeshes(RendererDX11* renderer, bool bClearGBuffer, const Meshes& meshes, size_t offset, size_t num, VertexShaderDX11* vertexShader, PixelShaderDX11* pixelShader, SamplerDX11* sampler, GBuffer* gBuffer, ConstantBufferDX11* transformBuffer, ConstantBufferDX11* materialParamsBuffer, RasterizerState* rasterizerState, Viewport* viewport, CameraRef camera, size_t threadIdx)
   {
      OPTICK_EVENT();
      {
         ID3D11DeviceContext& context = renderer->GetDeviceContext(threadIdx);
         context.ClearState();
         context.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

         vertexShader->Bind(context);
         pixelShader->Bind(context);
         sampler->Bind(context, 0);

         gBuffer->BindAsRenderTarget(context, bClearGBuffer, bClearGBuffer);
         transformBuffer->Bind(context, 0, EShaderType::VertexShader);
         materialParamsBuffer->Bind(context, 0, EShaderType::PixelShader);

         rasterizerState->Bind(context);
         viewport->Bind(context);

         auto camTransform = camera->GetTransform();
         Matrix viewMatrix = Matrix::CreateView(
            camTransform->GetPosition(ETransformSpace::World),
            camTransform->GetForward(ETransformSpace::World),
            camTransform->GetUp(ETransformSpace::World));
         Matrix projMatrix = Matrix::CreatePerspectiveProj(
            camera->GetFov(),
            (viewport->GetWidth() / (float)viewport->GetHeight()),
            camera->GetNearPlane(),
            camera->GetFarPlane());

         Material* material = nullptr;
         for (auto meshItr = meshes.begin() + offset; meshItr != meshes.begin() + offset + num; ++meshItr)
         {
            auto meshComponent = (*meshItr);
            Material* meshMaterial = meshComponent->GetMaterial();
            if (meshMaterial->GetMaterialType() == EMaterialType::Opaque)
            {
               if (material != meshMaterial)
               {
                  material = meshMaterial;
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

                  auto materialParams = materialParamsBuffer->Map<PackedMaterialParams>(context);
                  materialParams->BaseColorFactor = baseColorFactor;
                  materialParams->EmissiveColorFactor = emissiveFactor;
                  materialParams->MetallicRoughnessUV = Vector4(metallicFactor, roughnessFactor, uvOffset.x, uvOffset.y);
                  materialParams->SpecularFactor = specularFactor;
                  materialParamsBuffer->UnMap(context);

                  SAFE_TEX_BIND(baseColorTex, context, 0, EShaderType::PixelShader);
                  SAFE_TEX_BIND(emissiveTex, context, 1, EShaderType::PixelShader);
                  SAFE_TEX_BIND(metallicRoughnessTex, context, 2, EShaderType::PixelShader);
                  SAFE_TEX_BIND(specularMapTex, context, 3, EShaderType::PixelShader);
                  SAFE_TEX_BIND(aoTex, context, 4, EShaderType::PixelShader);
                  SAFE_TEX_BIND(normalTex, context, 5, EShaderType::PixelShader);
               }

               /** Render Mesh */
               Transform* transform = meshComponent->GetTransform();
               Mesh* mesh = meshComponent->GetMesh();
               Matrix worldMatrix = transform->GetWorldMatrix();
               Matrix worldViewMatrix = worldMatrix * viewMatrix;
               auto transforms = transformBuffer->Map<GeometryPassTransformBuffer>(context);
               transforms->WorldMatrix = worldMatrix;
               transforms->WorldViewMatrix = worldViewMatrix;
               transforms->WorldViewProjMatrix = (worldViewMatrix * projMatrix);
               transformBuffer->UnMap(context);
               mesh->Bind(context, 0);

               renderer->ThreadSafeDrawIndexed(threadIdx, mesh->GetVertexCount(), mesh->GetIndexCount());

               auto nextMeshItr = (meshItr + 1);
               if (nextMeshItr == meshes.end() || (*nextMeshItr)->GetMaterial() != material)
               {
                  Texture2dDX11* baseColorTex = material->GetTexture2D(MaterialTextureProperty::BaseColor)->GetRawTexture();
                  Texture2dDX11* emissiveTex = material->GetTexture2D(MaterialTextureProperty::Emissive)->GetRawTexture();
                  Texture2dDX11* metallicRoughnessTex = material->GetTexture2D(MaterialTextureProperty::MetallicRoughness)->GetRawTexture();
                  Texture2dDX11* specularMapTex = material->GetTexture2D(MaterialTextureProperty::Specular)->GetRawTexture();
                  Texture2dDX11* aoTex = material->GetTexture2D(MaterialTextureProperty::AO)->GetRawTexture();
                  Texture2dDX11* normalTex = material->GetTexture2D(MaterialTextureProperty::Normal)->GetRawTexture();
                  SAFE_TEX_UNBIND(baseColorTex, context, 0, EShaderType::PixelShader);
                  SAFE_TEX_UNBIND(emissiveTex, context, 1, EShaderType::PixelShader);
                  SAFE_TEX_UNBIND(metallicRoughnessTex, context, 2, EShaderType::PixelShader);
                  SAFE_TEX_UNBIND(specularMapTex, context, 3, EShaderType::PixelShader);
                  SAFE_TEX_UNBIND(aoTex, context, 4, EShaderType::PixelShader);
                  SAFE_TEX_UNBIND(normalTex, context, 5, EShaderType::PixelShader);
               }
            }
         }

         gBuffer->UnbindRenderTarget(context);
         transformBuffer->Unbind(context, 0, EShaderType::VertexShader);
         materialParamsBuffer->Unbind(context, 0, EShaderType::PixelShader);

         sampler->Unbind(context, 0);
         pixelShader->Unbind(context);
         vertexShader->Unbind(context);
      }
   }
}