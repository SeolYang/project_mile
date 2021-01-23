#include "Rendering/RendererDX11.h"
#include "Rendering/RasterizerState.h"
#include "Rendering/Viewport.h"
#include "Rendering/DepthStencilBufferDX11.h"
#include "Rendering/RenderTargetDX11.h"
#include "Rendering/Mesh.h"
#include "Rendering/Quad.h"
#include "Rendering/Cube.h"
#include "Rendering/GBuffer.h"
#include "Rendering/GeometryPass.h"
#include "Rendering/LightingPass.h"
#include "Rendering/Texture2dDX11.h"
#include "Rendering/BlendState.h"
#include "Rendering/DepthStencilState.h"
#include "Rendering/DynamicCubemap.h"
#include "Rendering/Equirect2CubemapPass.h"
#include "Rendering/IrradianceConvPass.h"
#include "Rendering/PrefilteringPass.h"
#include "Rendering/IntegrateBRDFPass.h"
#include "Rendering/ConvertGBufferToViewPass.h"
#include "Rendering/SSAOPass.h"
#include "Rendering/SSAOBlurPass.h"
#include "Rendering/AmbientEmissivePass.h"
#include "Rendering/SkyboxPass.h"
#include "Rendering/BoxBloomPass.h"
#include "Rendering/ExtractBrightnessPass.h"
#include "Rendering/GaussianBlurPass.h"
#include "Rendering/BlendingPass.h"
#include "Rendering/ToneMappingPass.h"
#include "Core/Context.h"
#include "Core/Window.h"
#include "Core/Logger.h"
#include "Component/MeshRenderComponent.h"
#include "Component/LightComponent.h"
#include "Component/CameraComponent.h"
#include "GameFramework/World.h"
#include "GameFramework/Entity.h"
#include "GameFramework/Transform.h"
#include "Resource/Material.h"
#include "Resource/Texture2D.h"
#include "Resource/RenderTexture.h"
#include "Resource/ResourceManager.h"
#include "Math/Vector2.h"
#include "MT/ThreadPool.h"
#include <random>

namespace Mile
{
   RendererDX11::RendererDX11(Context* context) :
      SubSystem(context),
      m_window(nullptr),
      m_device(nullptr),
      m_immediateContext(nullptr),
      m_vsyncEnabled(false),
      m_deferredContexts{ nullptr },
      m_swapChain(nullptr),
      m_renderTargetView(nullptr),
      m_depthStencilBuffer(nullptr),
      m_bDepthStencilEnabled(true),
      m_clearColor{ 0.0f, 0.0f, 0.0f, 1.0f },
      m_gBuffer(nullptr),
      m_geometryPass(nullptr),
      m_lightingPass(nullptr),
      m_lightingPassRenderBuffer(nullptr),
      m_skyboxPass(nullptr),
      m_envMap(nullptr),
      m_equirectToCubemapPass(nullptr),
      m_equirectangularMap(nullptr),
      m_cubeMesh(nullptr),
      m_irradianceConvPass(nullptr),
      m_irradianceMap(nullptr),
      m_prefilteringPass(nullptr),
      m_prefilterdEnvMap(nullptr),
      m_integrateBRDFPass(nullptr),
      m_brdfLUT(nullptr),
      m_convertGBufferToViewPass(nullptr),
      m_viewSpaceGBuffer(nullptr),
      m_bEnableSSAO(true),
      m_ssaoPass(nullptr),
      m_ssaoBlurPass(nullptr),
      m_ssaoNoise(nullptr),
      m_ssaoRadius(DEFAULT_SSAO_RADIUS),
      m_ssaoBias(DEFAULT_SSAO_BIAS),
      m_ssaoMagnitude(DEFAULT_SSAO_MAGNITUDE),
      m_noiseScale(Vector2(1.0f, 1.0f)),
      m_bCubemapDirtyFlag(true),
      m_bAlwaysComputeIBL(false),
      m_ambientEmissivePass(nullptr),
      m_ambientEmissivePassRenderBuffer(nullptr),
      m_aoFactor(DEFAULT_AO_FACTOR),
      m_hdrBuffer(nullptr), 
      m_bloomType(EBloomType::Gaussian),
      m_boxBloomPass(nullptr),
      m_gaussianBloomIntensity(DEFAULT_GAUSSIAN_BLOOM_INTENSITY),
      m_gaussianBloomAmount(DEFAULT_GAUSSIAN_BLOOM_AMOUNT),
      m_gaussianBloomThreshold(DEFAULT_GAUSSIAN_BLOOM_THRESHOLD),
      m_gaussianBloomDepthThreshold(DEFAULT_GAUSSIAN_BLOOM_DEPTH_THRESHOLD),
      m_extractBrightnessPass(nullptr),
      m_gaussianBlurPass(nullptr),
      m_blendingPass(nullptr),
      m_toneMappingPass(nullptr),
      m_exposureFactor(DEFAULT_EXPOSURE_FACTOR), 
      m_gammaFactor(DEFAULT_GAMMA_FACTOR),
      m_mainCamera(nullptr), 
      m_viewport(nullptr), 
      m_depthDisable(nullptr),
      m_defaultRasterizerState(nullptr), 
      m_noCulling(nullptr),
      m_additiveBlendState(nullptr),
      m_defaultBlendState(nullptr),
      m_depthLessEqual(nullptr),
      m_outputRenderTarget(nullptr)
   {
   }

   RendererDX11::~RendererDX11()
   {
      DeInit();
   }

   bool RendererDX11::Init()
   {
      Context* context = GetContext();
      if (SubSystem::Init())
      {
         if (InitAPI())
         {
            if (!InitPBR())
            {
               MELog(context, TEXT("RendererDX11"), ELogType::FATAL, TEXT("Failed to initialize PBR render pass."));
               return false;
            }

            if (!InitPostProcess())
            {
               MELog(context, TEXT("RendererDX11"), ELogType::FATAL, TEXT("Failed to initialize post-process pass."));
               return false;
            }

            if (!InitStates())
            {
               MELog(context, TEXT("RendererDX11"), ELogType::FATAL, TEXT("Failed to initialize render states."));
               return false;
            }
         }

         MELog(context, TEXT("RendererDX11"), ELogType::DEBUG, TEXT("RendererDX11 Initialized!"));
         SubSystem::InitSucceed();
         return true;
      }

      MELog(context, TEXT("RendererDX11"), ELogType::FATAL, TEXT("Failed to initialize RendererDX11."));
      return false;
   }

   bool RendererDX11::InitAPI()
   {
      Context* context = GetContext();
      /* Initialize low level rendering api **/
      m_window = context->GetSubSystem<Window>();
      Vector2 screenRes{ m_window->GetResolution() };
      if (m_window == nullptr)
      {
         MELog(context,
            TEXT("RendererDX11"),
            ELogType::FATAL,
            TEXT("Cannot found Window subsystem from Context."));
         return false;
      }

      if (!CreateDeviceAndSwapChain())
      {
         MELog(context,
            TEXT("RendererDX11"),
            ELogType::FATAL,
            TEXT("Failed to create Device and SwapChain."));
         return false;
      }

      if (!CreateDepthStencilBuffer())
      {
         MELog(context,
            TEXT("RendererDX11"),
            ELogType::FATAL,
            TEXT("Failed to create Depth-Stencil Buffer."));
         return false;
      }

      m_backBuffer = new RenderTargetDX11(this);
      if (!m_backBuffer->Init(m_renderTargetView, m_depthStencilBuffer))
      {
         return false;
      }

      m_screenQuad = new Quad(this);
      if (!m_screenQuad->Init(-1.0f, -1.0f, 1.0f, 1.0f))
      {
         MELog(context,
            TEXT("RendererDX11"),
            ELogType::FATAL,
            TEXT("Failed to create screen quad."));
         return false;
      }

      m_cubeMesh = new Cube(this);
      if (!m_cubeMesh->Init(Vector3(-1.0f, -1.0f, -1.0f), Vector3(1.0f, 1.0f, 1.0f)))
      {
         MELog(context,
            TEXT("RendererDX11"),
            ELogType::FATAL,
            TEXT("Failed to create Cube mesh."));
         return false;
      }

      // @TODO: Multiple viewports
      m_viewport = new Viewport(this);
      m_viewport->SetWidth(screenRes.x);
      m_viewport->SetHeight(screenRes.y);

      return true;
   }

   bool RendererDX11::InitPBR()
   {
      Context* context = GetContext();
      Vector2 screenRes{ m_window->GetResolution() };
      m_gBuffer = new GBuffer(this);
      if (!m_gBuffer->Init(
         static_cast<unsigned int>(screenRes.x),
         static_cast<unsigned int>(screenRes.y)))
      {
         MELog(context,
            TEXT("RendererDX11"),
            ELogType::FATAL,
            TEXT("Failed to create GBuffer."));
         return false;
      }
      m_gBuffer->SetDepthStencilBuffer(m_depthStencilBuffer);

      m_equirectToCubemapPass = new Equirect2CubemapPass(this);
      if (!m_equirectToCubemapPass->Init(DYNAMIC_CUBEMAP_SIZE))
      {
         MELog(context,
            TEXT("RendererDX11"),
            ELogType::FATAL,
            TEXT("Failed to create Equirectangular to cubemap convert pass."));
         return false;
      }

      m_irradianceConvPass = new IrradianceConvPass(this);
      if (!m_irradianceConvPass->Init(IRRADIANCEMAP_SIZE))
      {
         MELog(context,
            TEXT("RendererDX11"),
            ELogType::FATAL,
            TEXT("Failed to create Irradiance Convolution pass."));
         return false;
      }

      m_prefilteringPass = new PrefilteringPass(this);
      if (!m_prefilteringPass->Init(PREFILTERED_CUBEMAP_SIZE))
      {
         MELog(context,
            TEXT("RendererDX11"),
            ELogType::FATAL,
            TEXT("Failed to create Prefiltering pass."));
         return false;
      }

      m_integrateBRDFPass = new IntegrateBRDFPass(this);
      if (!m_integrateBRDFPass->Init())
      {
         MELog(context,
            TEXT("RendererDX11"),
            ELogType::FATAL,
            TEXT("Failed to create Integrate brdf pass."));
         return false;
      }

      m_geometryPass = new GeometryPass(this);
      if (!m_geometryPass->Init())
      {
         MELog(context,
            TEXT("RendererDX11"),
            ELogType::FATAL,
            TEXT("Failed to create Geometry Pass."));
         return false;
      }

      m_lightingPass = new LightingPass(this);
      if (!m_lightingPass->Init())
      {
         MELog(context,
            TEXT("RendererDX11"),
            ELogType::FATAL,
            TEXT("Failed to create Lighting Pass."));
         return false;
      }

      return true;
   }

   bool RendererDX11::InitSSAO()
   {
      Context* context = GetContext();
      Vector2 screenRes{ m_window->GetResolution() };
      SSAOPass::SSAOParams ssaoParams;
      ssaoParams.NoiseScale = Vector2(screenRes.x / SSAO_NOISE_TEXTURE_RES, screenRes.y / SSAO_NOISE_TEXTURE_RES);

      std::uniform_real_distribution<float> randomFloats(0.0f, 1.0f);
      std::default_random_engine generator;
      for (unsigned int idx = 0; idx < SSAO_KERNEL_SIZE; ++idx)
      {
         Vector4 sample{
            randomFloats(generator) * 2.0f - 1.0f,
            randomFloats(generator) * 2.0f - 1.0f,
            randomFloats(generator) /* Hemisphere : z = [0, 1] */,
            0.0f };
         sample.Normalize();
         sample *= randomFloats(generator);

         float scale = (float)idx / (float)SSAO_KERNEL_SIZE;
         scale = Math::Lerp(0.1f, 1.0f, scale * scale);
         sample *= scale;

         ssaoParams.Samples[idx] = sample;
      }

      std::vector<Vector4> ssaoNoise;
      for (unsigned int idx = 0; idx < static_cast<unsigned int>(SSAO_NOISE_TEXTURE_RES * SSAO_NOISE_TEXTURE_RES); ++idx)
      {
         ssaoNoise.push_back(Vector4(
            randomFloats(generator) * 2.0f - 1.0f,
            randomFloats(generator) * 2.0f - 1.0f,
            0.0f,
            0.0f));
      }

      m_ssaoNoise = new Texture2dDX11(this);
      if (!m_ssaoNoise->Init(
         static_cast<unsigned int>(SSAO_NOISE_TEXTURE_RES), static_cast<unsigned int>(SSAO_NOISE_TEXTURE_RES), 4,
         (unsigned char*)ssaoNoise.data(),
         DXGI_FORMAT_R32G32B32A32_FLOAT))
      {
         MELog(context, TEXT("RendererDX11"), ELogType::FATAL, TEXT("Failed to initialize SSAO Noise texture."));
         return false;
      }

      m_ssaoPass = new SSAOPass(this);
      if (!m_ssaoPass->Init(
         static_cast<unsigned int>(screenRes.x), 
         static_cast<unsigned int>(screenRes.y),
         m_depthStencilBuffer))
      {
         MELog(context, TEXT("RendererDX11"), ELogType::FATAL, TEXT("Failed to initialize SSAO pass."));
         return false;
      }

      m_ssaoPass->UpdateParameters(*GetRenderContextByType(ERenderContextType::Immediate), ssaoParams);

      m_ssaoBlurPass = new SSAOBlurPass(this);
      if (!m_ssaoBlurPass->Init(
         static_cast<unsigned int>(screenRes.x),
         static_cast<unsigned int>(screenRes.y),
         m_depthStencilBuffer))
      {
         MELog(context, TEXT("RendererDX11"), ELogType::FATAL, TEXT("Failed to initialize SSAO Blur pass."));
         return false;
      }

      return true;
   }

   bool RendererDX11::InitPostProcess()
   {
      Context* context = GetContext();
      Vector2 screenRes{ m_window->GetResolution() };

      m_hdrBuffer = new RenderTargetDX11(this);
      if (!m_hdrBuffer->Init(
         static_cast<unsigned int>(screenRes.x),
         static_cast<unsigned int>(screenRes.y),
         DXGI_FORMAT_R16G16B16A16_FLOAT,
         m_depthStencilBuffer))
      {
         MELog(context,
            TEXT("RendererDX11"),
            ELogType::FATAL,
            TEXT("Failed to create HDR Buffer."));
         return false;
      }

      m_convertGBufferToViewPass = new ConvertGBufferToViewPass(this);
      if (!m_convertGBufferToViewPass->Init(
         static_cast<unsigned int>(screenRes.x),
         static_cast<unsigned int>(screenRes.y),
         m_depthStencilBuffer))
      {
         MELog(context,
            TEXT("RendererDX11"),
            ELogType::FATAL,
            TEXT("Failed to create Convert GBuffer to view space pass."));
         return false;
      }
      m_viewSpaceGBuffer = m_convertGBufferToViewPass->GetConvertedGBuffer();

      m_ambientEmissivePass = new AmbientEmissivePass(this);
      if (!m_ambientEmissivePass->Init())
      {
         MELog(context,
            TEXT("RendererDX11"),
            ELogType::FATAL,
            TEXT("Failed to create ambient emissive pass."));
         return false;
      }

      m_skyboxPass = new SkyboxPass(this);
      if (!m_skyboxPass->Init())
      {
         MELog(context,
            TEXT("RendererDX11"),
            ELogType::FATAL,
            TEXT("Failed to create Skybox Pass."));
         return false;
      }

      m_boxBloomPass = new BoxBloomPass(this);
      if (!m_boxBloomPass->Init(
         static_cast<unsigned int>(screenRes.x),
         static_cast<unsigned int>(screenRes.y),
         m_depthStencilBuffer))
      {
         MELog(context,
            TEXT("RendererDX11"),
            ELogType::FATAL,
            TEXT("Failed to create Box Bloom pass."));
         return false;
      }

      m_extractBrightnessPass = new ExtractBrightnessPass(this);
      if (!m_extractBrightnessPass->Init(
         static_cast<unsigned int>(screenRes.x),
         static_cast<unsigned int>(screenRes.y),
         m_depthStencilBuffer))
      {
         MELog(context,
            TEXT("RendererDX11"),
            ELogType::FATAL,
            TEXT("Failed to create Extract brightness pass."));
         return false;
      }

      m_gaussianBlurPass = new GaussianBlurPass(this);
      if (!m_gaussianBlurPass->Init(
         static_cast<unsigned int>(screenRes.x),
         static_cast<unsigned int>(screenRes.y),
         m_depthStencilBuffer))
      {
         MELog(context,
            TEXT("RendererDX11"),
            ELogType::FATAL,
            TEXT("Failed to create Gaussian blur pass."));
         return false;
      }

      m_blendingPass = new BlendingPass(this);
      if (!m_blendingPass->Init(
         static_cast<unsigned int>(screenRes.x),
         static_cast<unsigned int>(screenRes.y),
         m_depthStencilBuffer))
      {
         MELog(context,
            TEXT("RendererDX11"),
            ELogType::FATAL,
            TEXT("Failed to create Blending pass."));
         return false;
      }

      m_toneMappingPass = new ToneMappingPass(this);
      if (!m_toneMappingPass->Init())
      {
         MELog(context,
            TEXT("RendererDX11"),
            ELogType::FATAL,
            TEXT("Failed to create Tone mapping pass."));
         return false;
      }

      if (!InitSSAO())
      {
         MELog(context, TEXT("RendererDX11"), ELogType::FATAL, TEXT("Failed to initialize SSAO pass."));
         return false;
      }

      return true;
   }

   bool RendererDX11::InitStates()
   {
      Context* context = GetContext();
      m_defaultRasterizerState = new RasterizerState(this);
      if (!m_defaultRasterizerState->Init())
      {
         MELog(context,
            TEXT("RendererDX11"),
            ELogType::FATAL,
            TEXT("Failed to create Default RasteirzerState."));
         return false;
      }

      m_noCulling = new RasterizerState(this);
      m_noCulling->SetCullMode(CullMode::NONE);
      if (!m_noCulling->Init())
      {
         MELog(context,
            TEXT("RendererDX11"),
            ELogType::FATAL,
            TEXT("Failed to create no culling rasterizer state."));
         return false;
      }

      m_defaultBlendState = new BlendState(this);
      if (!m_defaultBlendState->Init())
      {
         MELog(context,
            TEXT("RendererDX11"),
            ELogType::FATAL,
            TEXT("Failed to create default blend state."));
         return false;
      }

      m_additiveBlendState = new BlendState(this);
      if (!m_additiveBlendState->Init())
      {
         MELog(context,
            TEXT("RendererDX11"),
            ELogType::FATAL,
            TEXT("Failed to create additive blend state."));
         return false;
      }

      m_additiveBlendState->SetRenderTargetBlendState(
         {
            true,
            EBlend::ONE, EBlend::ONE, EBlendOP::ADD,
            EBlend::ONE, EBlend::ZERO, EBlendOP::ADD,
            (UINT8)EColorWriteEnable::ColorWriteEnableAll
         });

      m_depthLessEqual = new DepthStencilState(this);
      D3D11_DEPTH_STENCIL_DESC depthLessEqualDesc = DepthStencilState::GetDefaultDesc();
      depthLessEqualDesc.DepthEnable = true;
      depthLessEqualDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
      m_depthLessEqual->SetDesc(depthLessEqualDesc);
      if (!m_depthLessEqual->Init())
      {
         MELog(context,
            TEXT("RendererDX11"),
            ELogType::FATAL,
            TEXT("Failed to create DEPTH LESS EQUAL state."));
         return false;
      }

      m_depthDisable = new DepthStencilState(this);
      D3D11_DEPTH_STENCIL_DESC depthDisableDesc = DepthStencilState::GetDefaultDesc();
      depthDisableDesc.DepthEnable = false;
      m_depthDisable->SetDesc(depthDisableDesc);
      if (!m_depthDisable->Init())
      {
         MELog(context,
            TEXT("RendererDX11"),
            ELogType::FATAL,
            TEXT("Failed to create DEPTH DISALBE state."));
         return false;
      }

      return true;
   }

   void RendererDX11::DeInit()
   {
      if (IsInitialized())
      {
         SafeDelete(m_viewport);
         SafeDelete(m_depthDisable);
         SafeDelete(m_depthLessEqual);
         SafeDelete(m_additiveBlendState);
         SafeDelete(m_defaultBlendState);
         SafeDelete(m_defaultRasterizerState);
         SafeDelete(m_noCulling);
         SafeDelete(m_screenQuad);
         SafeDelete(m_cubeMesh);
         SafeDelete(m_equirectToCubemapPass);
         SafeDelete(m_irradianceConvPass);
         SafeDelete(m_prefilteringPass);
         SafeDelete(m_integrateBRDFPass);
         SafeDelete(m_geometryPass);
         SafeDelete(m_lightingPass);
         SafeDelete(m_lightingPassRenderBuffer);
         SafeDelete(m_ambientEmissivePass);
         SafeDelete(m_ambientEmissivePassRenderBuffer);
         SafeDelete(m_skyboxPass);
         SafeDelete(m_toneMappingPass);
         SafeDelete(m_boxBloomPass);
         SafeDelete(m_extractBrightnessPass);
         SafeDelete(m_gaussianBlurPass);
         SafeDelete(m_blendingPass);
         SafeDelete(m_gBuffer);
         SafeDelete(m_convertGBufferToViewPass);
         SafeDelete(m_ssaoNoise);
         SafeDelete(m_ssaoPass);
         SafeDelete(m_ssaoBlurPass);
         SafeDelete(m_backBuffer);
         SafeDelete(m_depthStencilBuffer);
         SafeRelease(m_swapChain);

         for (auto deferredContext : m_deferredContexts)
         {
            SafeRelease(deferredContext);
         }

         SafeRelease(m_immediateContext);
         SafeRelease(m_device);

         SubSystem::DeInit();
         MELog(GetContext(), TEXT("RendererDX11"), ELogType::DEBUG, TEXT("RendererDX11 deinitialized."));
      }
   }

   void RendererDX11::SetVsync(bool enabled)
   {
      m_vsyncEnabled = enabled;
   }

   bool RendererDX11::CreateDeviceAndSwapChain()
   {
      DXGI_SWAP_CHAIN_DESC swDesc;
      ZeroMemory(&swDesc, sizeof(swDesc));

      swDesc.BufferCount = 1;
      swDesc.BufferDesc.Width = m_window->GetResWidth();
      swDesc.BufferDesc.Height = m_window->GetResHeight();
      swDesc.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
      swDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
      swDesc.OutputWindow = reinterpret_cast<HWND>(m_window->GetHandle());
      swDesc.SampleDesc.Count = 1;
      swDesc.SampleDesc.Quality = 0;
      swDesc.Windowed = !m_window->IsFullScreen();

      unsigned int createDeviceFlags = 0;
#ifdef _DEBUG
      createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

      D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0 };
      unsigned int numOfFeatureLevels = ARRAYSIZE(featureLevels);
      D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0;

      auto hr = D3D11CreateDeviceAndSwapChain(
         nullptr,
         D3D_DRIVER_TYPE_HARDWARE,
         NULL,
         createDeviceFlags,
         featureLevels,
         numOfFeatureLevels,
         D3D11_SDK_VERSION,
         &swDesc,
         &m_swapChain,
         &m_device,
         &featureLevel,
         &m_immediateContext);

      if (FAILED(hr))
      {
         /* Failed to create device and swap chain. **/
         return false;
      }

      /* Create Render target view **/
      ID3D11Texture2D* backBuffer = nullptr;
      hr = m_swapChain->GetBuffer(
         0,
         __uuidof(ID3D11Texture2D),
         (LPVOID*)&backBuffer);

      if (FAILED(hr))
      {
         /* Failed to get back buffer texture resource from swap chain. **/
         return false;
      }

      hr = m_device->CreateRenderTargetView(
         backBuffer,
         nullptr,
         &m_renderTargetView);
      SafeRelease(backBuffer);

      if (FAILED(hr))
      {
         /* Failed to create render target view with backbuffer. **/
         return false;
      }

      // Create Deferred Contexts
      for (auto idx = 0; idx < REQUIRED_RENDERCONTEXT_NUM; ++idx)
      {
         hr = m_device->CreateDeferredContext(0, &m_deferredContexts[idx]);
         if (FAILED(hr))
         {
            // Failed to create deferred contexts.
            return false;
         }
      }

      return true;
   }

   bool RendererDX11::CreateDepthStencilBuffer()
   {
      if (m_device == nullptr)
      {
         return false;
      }

      m_depthStencilBuffer = new DepthStencilBufferDX11(this);
      bool res = m_depthStencilBuffer->Init(m_window->GetResWidth(),
         m_window->GetResHeight(),
         true);

      if (!res)
      {
         return false;
      }

      return true;
   }

   void RendererDX11::AcquireMeshRenderersAndMaterial(World* world)
   {
      if (world != nullptr)
      {
         m_meshRenderComponents.clear();
         m_materialMap.clear();

         m_meshRenderComponents = std::move(world->GetComponentsFromEntities<MeshRenderComponent>());
         for (auto renderComponent : m_meshRenderComponents)
         {
            Material* material = renderComponent->GetMaterial();
            if (material != nullptr) /** nullptr 일때 기본 material 필요할까? */
            {
               m_materialMap[material].push_back(renderComponent);
            }
         }
      }
   }

   void RendererDX11::AcquireLights(World* world)
   {
      if (world != nullptr)
      {
         m_lightComponents.clear();
         m_lightComponents = std::move(world->GetComponentsFromEntities<LightComponent>());
      }
   }

   void RendererDX11::AcquireCameras(World* world)
   {
      m_cameras.clear();
      if (world != nullptr)
      {
         m_cameras = std::move(world->GetComponentsFromEntities<CameraComponent>());
      }
   }

   void RendererDX11::Render()
   {
      Context* context = GetContext();
      auto threadPool = context->GetSubSystem<ThreadPool>();
      Clear( *m_immediateContext );
      m_bIsRendered = false;

      World* world = context->GetSubSystem<World>();
      if (world != nullptr)
      {
         /* Pre-process part**/
         auto acquireMeshRenderersAndMatBinder = std::bind(&RendererDX11::AcquireMeshRenderersAndMaterial,
            this,
            world);
         auto acquireMeshRenderersAndMatTask = threadPool->AddTask(acquireMeshRenderersAndMatBinder);

         auto acquireLightBinder = std::bind(&RendererDX11::AcquireLights, this, world);
         auto acquireLightTask = threadPool->AddTask(acquireLightBinder);

         auto acquireCamerasBinder = std::bind(&RendererDX11::AcquireCameras, this, world);
         auto acquireCamerasTask = threadPool->AddTask(acquireCamerasBinder);

         acquireMeshRenderersAndMatTask.get();
         acquireLightTask.get();
         acquireCamerasTask.get();

         // @TODO: Implement Multiple camera rendering
         for (CameraComponent* camera : m_cameras)
         {
            m_mainCamera = camera;
            RenderTexture* renderTexture = m_mainCamera->GetRenderTexture();
            if (renderTexture != nullptr)
            {
               m_outputRenderTarget = renderTexture->GetRenderTarget();
            }
            else
            {
#ifdef MILE_EDITOR
               if (renderTexture != nullptr)
               {
                  ResourceManager* resMng = Engine::GetResourceManager();
                  renderTexture = resMng->Load<RenderTexture>(EDITOR_GAME_VIEW_RENDER_TEXTURE, true);
                  m_outputRenderTarget = renderTexture->GetRenderTarget();
               }
               else
               {
                  m_outputRenderTarget = nullptr;
               }
#else
               m_outputRenderTarget = m_backBuffer;
#endif
            }

            if (m_outputRenderTarget != nullptr)
            {
               m_viewport->SetWidth(m_backBuffer->GetWidth());
               m_viewport->SetHeight(m_backBuffer->GetHeight());

               /* Main Rendering part **/
               /* PBS Workflow **/
               auto geometryPassBinder = std::bind(
                  &RendererDX11::RunGeometryPass,
                  this,
                  GetRenderContextByType(ERenderContextType::GeometryPass));
               auto geometryPassTask = threadPool->AddTask(geometryPassBinder);

               auto lightingPassBinder = std::bind(
                  &RendererDX11::RunLightingPass,
                  this,
                  GetRenderContextByType(ERenderContextType::LightingPass));
               auto lightingPassTask = threadPool->AddTask(lightingPassBinder);

               ID3D11CommandList* geometryPassCmdList = geometryPassTask.get();
               ID3D11CommandList* lightingPassCmdList = lightingPassTask.get();
               ID3D11CommandList* postProcessCmdList = RunPostProcessPass(GetRenderContextByType(ERenderContextType::PostProcessPass));

               SAFE_EXECUTE_CMDLIST(m_immediateContext, geometryPassCmdList, false);
               SAFE_EXECUTE_CMDLIST(m_immediateContext, lightingPassCmdList, false);
               SAFE_EXECUTE_CMDLIST(m_immediateContext, postProcessCmdList, false);

               SafeRelease(geometryPassCmdList);
               SafeRelease(lightingPassCmdList);
               SafeRelease(postProcessCmdList);

               m_bIsRendered = true;
            }
         }
      }
   }

   ID3D11CommandList* RendererDX11::RunGeometryPass(ID3D11DeviceContext* deviceContextPtr)
   {
      if (deviceContextPtr != nullptr)
      {
         ID3D11DeviceContext& deviceContext = *deviceContextPtr;
         m_geometryPass->SetGBuffer(m_gBuffer);
         if (m_geometryPass->Bind(deviceContext))
         {
            m_defaultRasterizerState->Bind(deviceContext);
            m_viewport->Bind(deviceContext);

            auto camTransform = m_mainCamera->GetTransform();
            Matrix viewMatrix = Matrix::CreateView(
               camTransform->GetPosition(TransformSpace::World),
               camTransform->GetForward(TransformSpace::World),
               camTransform->GetUp(TransformSpace::World));
            Matrix projMatrix = Matrix::CreatePerspectiveProj(
               m_mainCamera->GetFov(),
               m_window->GetAspectRatio(),
               m_mainCamera->GetNearPlane(),
               m_mainCamera->GetFarPlane());

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

                     m_geometryPass->UpdateMaterialBuffer(
                        deviceContext,
                        { baseColorFactor, emissiveFactor, metallicFactor, roughnessFactor, uvOffset, specularFactor });

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

                        Matrix worldMatrix = transform->GetWorldMatrix();

                        Matrix worldViewMatrix = worldMatrix * viewMatrix;
                        m_geometryPass->UpdateTransformBuffer(
                           deviceContext,
                           { worldMatrix, worldViewMatrix, worldViewMatrix * projMatrix });

                        mesh->Bind(deviceContext, 0);
                        deviceContext.DrawIndexed(mesh->GetIndexCount(), 0, 0);
                     }

                     SAFE_TEX_UNBIND(baseColorTex, deviceContext);
                     SAFE_TEX_UNBIND(emissiveTex, deviceContext);
                     SAFE_TEX_UNBIND(metallicRoughnessTex, deviceContext);
                     SAFE_TEX_UNBIND(specularMapTex, deviceContext);
                     SAFE_TEX_UNBIND(aoTex, deviceContext);
                     SAFE_TEX_UNBIND(normalTex, deviceContext);
                     break;
                  }
               }
            }

            m_geometryPass->Unbind(deviceContext);
         }

         ID3D11CommandList* commandList = nullptr;
         auto result = deviceContext.FinishCommandList(false, &commandList);
         if (!FAILED(result))
         {
            return commandList;
         }
      }

      return nullptr;
   }

   ID3D11CommandList* RendererDX11::RunLightingPass(ID3D11DeviceContext* deviceContextPtr)
   {
      if (deviceContextPtr != nullptr)
      {
         ID3D11DeviceContext& deviceContext = *deviceContextPtr;

         CalculateDiffuseIrradiance(deviceContext);
         RenderLight(deviceContext);

         ID3D11CommandList* commandList = nullptr;
         auto result = deviceContext.FinishCommandList(false, &commandList);
         if (!FAILED(result))
         {
            return commandList;
         }
      }

      return nullptr;
   }

   ID3D11CommandList* RendererDX11::RunPostProcessPass(ID3D11DeviceContext* deviceContextPtr)
   {
      if (deviceContextPtr != nullptr)
      {
         ID3D11DeviceContext& deviceContext = *deviceContextPtr;
         m_viewSpaceGBuffer = ConvertGBufferToViewSpace(deviceContext, m_gBuffer);

         RenderTargetDX11* output = RenderAmbientEmissive(deviceContext, m_hdrBuffer);
         output = Bloom(deviceContext, output);
         output = RenderSkybox(deviceContext, output);

         ToneMappingWithGammaCorrection(deviceContext, output);

         ID3D11CommandList* commandList = nullptr;
         auto result = deviceContext.FinishCommandList(false, &commandList);
         if (!FAILED(result))
         {
            return commandList;
         }
      }

      return nullptr;
   }

   void RendererDX11::CalculateDiffuseIrradiance(ID3D11DeviceContext& deviceContext)
   {
      if (m_bCubemapDirtyFlag || m_bAlwaysComputeIBL)
      {
         static const Matrix captureProj = Matrix::CreatePerspectiveProj(90.0f, 1.0f, 0.1f, 10.0f);
         static const std::array<Matrix, CUBE_FACES> captureMatrix =
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

         ConvertEquirectToCubemap(deviceContext, captureMatrix);
         SolveDiffuseIntegral(deviceContext, captureMatrix);
         ComputePrefilteredEnvMap(deviceContext, captureMatrix);
         IntegrateBRDF(deviceContext);
         m_bCubemapDirtyFlag = false;
      }
   }

   void RendererDX11::ConvertEquirectToCubemap(ID3D11DeviceContext& deviceContext, const std::array<Matrix, CUBE_FACES>& captureMatrix)
   {
      m_envMap = m_equirectToCubemapPass->GetCubemap();
      if (m_equirectToCubemapPass->Bind(deviceContext, (m_equirectangularMap != nullptr) ? m_equirectangularMap->GetRawTexture() : nullptr))
      {
         m_depthLessEqual->Bind(deviceContext);
         m_noCulling->Bind(deviceContext);
         m_cubeMesh->Bind(deviceContext, 0);
         for (unsigned int faceIndex = 0; faceIndex < CUBE_FACES; ++faceIndex)
         {
            m_envMap->BindAsRenderTarget(deviceContext, faceIndex);
            m_equirectToCubemapPass->UpdateTransformBuffer(
               deviceContext,
               { captureMatrix[faceIndex] });
            deviceContext.DrawIndexed(m_cubeMesh->GetIndexCount(), 0, 0);
            m_envMap->UnbindAsRenderTarget(deviceContext);
         }

         m_envMap->GenerateMips(deviceContext);
         m_equirectToCubemapPass->Unbind(deviceContext);
      }
   }

   void RendererDX11::SolveDiffuseIntegral(ID3D11DeviceContext& deviceContext, const std::array<Matrix, CUBE_FACES>& captureMatrix)
   {
      m_irradianceMap = m_irradianceConvPass->GetIrradianceMap();
      if (m_irradianceConvPass->Bind(deviceContext, m_envMap))
      {
         m_depthLessEqual->Bind(deviceContext);
         m_noCulling->Bind(deviceContext);
         m_cubeMesh->Bind(deviceContext, 0);
         for (unsigned int faceIndex = 0; faceIndex < CUBE_FACES; ++faceIndex)
         {
            m_irradianceMap->BindAsRenderTarget(deviceContext, faceIndex);
            m_irradianceConvPass->UpdateTransformBuffer(
               deviceContext,
               { captureMatrix[faceIndex] });
            deviceContext.DrawIndexed(m_cubeMesh->GetIndexCount(), 0, 0);
            m_irradianceMap->UnbindAsRenderTarget(deviceContext);
         }

         m_irradianceMap->GenerateMips(deviceContext);
         m_irradianceConvPass->Unbind(deviceContext);
      }
   }

   void RendererDX11::ComputePrefilteredEnvMap(ID3D11DeviceContext& deviceContext, const std::array<Matrix, CUBE_FACES>& captureMatrix)
   {
      m_prefilterdEnvMap = m_prefilteringPass->GetPrefilteredEnvironmentMap();
      m_prefilterdEnvMap->GenerateMips(deviceContext);
      if (m_prefilteringPass->Bind(deviceContext, m_envMap))
      {
         /** Generate Empty mips */
         for (unsigned int mipLevel = 0; mipLevel < PREFILTERED_CUBEMAP_MAX_MIPLEVELS; ++mipLevel)
         {
            Viewport* viewport = m_prefilteringPass->GetViewport(mipLevel);
            viewport->Bind(deviceContext);
            m_depthLessEqual->Bind(deviceContext);
            m_noCulling->Bind(deviceContext);
            m_cubeMesh->Bind(deviceContext, 0);

            float roughness = static_cast<float>(mipLevel) / static_cast<float>(PREFILTERED_CUBEMAP_MAX_MIPLEVELS - 1);
            for (unsigned int faceIdx = 0; faceIdx < CUBE_FACES; ++faceIdx)
            {
               m_prefilteringPass->UpdateTransformBuffer(
                  deviceContext, 
                  { captureMatrix[faceIdx] });
               m_prefilteringPass->UpdatePrefilteringParams(
                  deviceContext, 
                  { roughness });
               m_prefilterdEnvMap->BindAsRenderTarget(deviceContext, faceIdx, mipLevel);

               deviceContext.DrawIndexed(m_cubeMesh->GetIndexCount(), 0, 0);

               m_prefilterdEnvMap->UnbindAsRenderTarget(deviceContext);
            }
         }

         m_prefilteringPass->Unbind(deviceContext);
      }
   }

   void RendererDX11::IntegrateBRDF(ID3D11DeviceContext& deviceContext)
   {
      m_brdfLUT = m_integrateBRDFPass->GetBrdfLUT();
      if (m_integrateBRDFPass->Bind(deviceContext))
      {
         m_screenQuad->Bind(deviceContext, 0);
         m_depthDisable->Bind(deviceContext);

         deviceContext.DrawIndexed(m_screenQuad->GetIndexCount(), 0, 0);

         m_integrateBRDFPass->Unbind(deviceContext);
      }
   }

   void RendererDX11::RenderLight(ID3D11DeviceContext& deviceContext)
   {
      Transform* camTransform = m_mainCamera->GetTransform();
      m_lightingPass->SetGBuffer(m_gBuffer);
      if (m_lightingPass->Bind(deviceContext))
      {
         m_hdrBuffer->BindAsRenderTarget(deviceContext, true, false);
         m_depthDisable->Bind(deviceContext);
         m_viewport->Bind(deviceContext);
         m_defaultRasterizerState->Bind(deviceContext);
         m_additiveBlendState->Bind(deviceContext);
         m_screenQuad->Bind(deviceContext, 0);

         for (auto lightComponent : m_lightComponents)
         {
            m_lightingPass->UpdateCameraParamsBuffer(
               deviceContext,
               { camTransform->GetPosition(TransformSpace::World) });

            Transform* lightTransform = lightComponent->GetTransform();
            Vector3 lightPosition = lightTransform->GetPosition(TransformSpace::World);
            Vector3 lightDirection = lightTransform->GetForward(TransformSpace::World);
            Vector3 lightRadiance = lightComponent->GetRadiance();
            m_lightingPass->UpdateLightParamsBuffer(
               deviceContext,
               {
                  Vector4(lightPosition.x, lightPosition.y, lightPosition.z, 1.0f),
                  Vector4(lightDirection.x, lightDirection.y, lightDirection.z, 0.0f),
                  Vector4(lightRadiance.x, lightRadiance.y, lightRadiance.z, 1.0f),
                  static_cast<UINT32>(lightComponent->GetLightType())
               });

            deviceContext.DrawIndexed(m_screenQuad->GetIndexCount(), 0, 0);
         }

         m_hdrBuffer->UnbindRenderTarget(deviceContext);
         m_lightingPass->Unbind(deviceContext);
      }
   }

   GBuffer* RendererDX11::ConvertGBufferToViewSpace(ID3D11DeviceContext& deviceContext, GBuffer* gBuffer)
   {
      if (gBuffer != nullptr)
      {
         auto camTransform = m_mainCamera->GetTransform();
         Matrix viewMatrix = Matrix::CreateView(
            camTransform->GetPosition(TransformSpace::World),
            camTransform->GetForward(TransformSpace::World),
            camTransform->GetUp(TransformSpace::World));

         if (m_convertGBufferToViewPass->Bind(deviceContext, gBuffer))
         {
            m_depthDisable->Bind(deviceContext);
            m_viewport->Bind(deviceContext);
            m_defaultRasterizerState->Bind(deviceContext);
            m_screenQuad->Bind(deviceContext, 0);

            m_convertGBufferToViewPass->UpdateParameters(deviceContext,
               {
                  viewMatrix
               });
            deviceContext.DrawIndexed(m_screenQuad->GetIndexCount(), 0, 0);

            m_convertGBufferToViewPass->Unbind(deviceContext);
         }

         return m_convertGBufferToViewPass->GetConvertedGBuffer();
      }

      return nullptr;
   }

   RenderTargetDX11* RendererDX11::RenderAmbientEmissive(ID3D11DeviceContext& deviceContext, RenderTargetDX11* renderTarget)
   {
      if (renderTarget != nullptr)
      {
         RenderTargetDX11* aoBuffer = nullptr;
         if (m_bEnableSSAO)
         {
            aoBuffer = SSAO(deviceContext, m_viewSpaceGBuffer, m_ssaoRadius, m_ssaoBias);
            aoBuffer = SSAOBlur(deviceContext, aoBuffer);
         }

         Transform* camTransform = m_mainCamera->GetTransform();
         m_ambientEmissivePass->SetGBuffer(m_gBuffer);
         m_ambientEmissivePass->SetIrradianceMap(m_irradianceConvPass->GetIrradianceMap());
         m_ambientEmissivePass->SetPrefilteredMap(m_prefilterdEnvMap);
         m_ambientEmissivePass->SetBRDFLUT(m_brdfLUT);
         if (m_ambientEmissivePass->Bind(deviceContext))
         {
            renderTarget->BindAsRenderTarget(deviceContext, false, false);
            m_depthDisable->Bind(deviceContext);
            m_viewport->Bind(deviceContext);
            m_defaultRasterizerState->Bind(deviceContext);
            m_additiveBlendState->Bind(deviceContext);
            m_screenQuad->Bind(deviceContext, 0);

            if (aoBuffer != nullptr)
            {
               aoBuffer->BindAsShaderResource(deviceContext, 8, EShaderType::PixelShader);
            }

            m_ambientEmissivePass->UpdateAmbientParamsBuffer(
               deviceContext,
               {
                  camTransform->GetPosition(TransformSpace::World),
                  m_aoFactor, static_cast<unsigned int>(m_bEnableSSAO ? 1 : 0)
               });

            deviceContext.DrawIndexed(m_screenQuad->GetIndexCount(), 0, 0);

            if (aoBuffer != nullptr)
            {
               aoBuffer->UnbindShaderResource(deviceContext);
            }

            renderTarget->UnbindRenderTarget(deviceContext);
            m_ambientEmissivePass->Unbind(deviceContext);
         }

         return renderTarget;
      }

      return nullptr;
   }

   RenderTargetDX11* RendererDX11::RenderSkybox(ID3D11DeviceContext& deviceContext, RenderTargetDX11* renderTarget)
   {
      if (renderTarget != nullptr)
      {
         Transform* camTransform = m_mainCamera->GetTransform();
         if (m_skyboxPass->Bind(deviceContext, m_equirectToCubemapPass->GetCubemap()))
         {
            renderTarget->BindAsRenderTarget(deviceContext, false, false);
            m_depthLessEqual->Bind(deviceContext);
            m_viewport->Bind(deviceContext);
            m_noCulling->Bind(deviceContext);

            Matrix viewMatrix = Matrix::CreateView(
               Vector3(0.0f, 0.0f, 0.0f),
               camTransform->GetForward(TransformSpace::World),
               camTransform->GetUp(TransformSpace::World));
            Matrix projMatrix = Matrix::CreatePerspectiveProj(
               m_mainCamera->GetFov(),
               m_window->GetAspectRatio(),
               0.1f,
               10.0f);

            m_skyboxPass->UpdateTransformBuffer(
               deviceContext,
               {
                  viewMatrix * projMatrix
               });

            m_cubeMesh->Bind(deviceContext, 0);
            deviceContext.DrawIndexed(m_cubeMesh->GetIndexCount(), 0, 0);
            renderTarget->UnbindRenderTarget(deviceContext);
            m_skyboxPass->Unbind(deviceContext);
         }

         return renderTarget;
      }

      return nullptr;
   }

   RenderTargetDX11* RendererDX11::SSAO(ID3D11DeviceContext& deviceContext, GBuffer* gBuffer, float radius, float bias)
   {
      if (m_ssaoPass->Bind(deviceContext, gBuffer, m_ssaoNoise))
      {
         m_depthDisable->Bind(deviceContext);
         m_viewport->Bind(deviceContext);
         m_defaultRasterizerState->Bind(deviceContext);
         m_screenQuad->Bind(deviceContext, 0);

         auto camTransform = m_mainCamera->GetTransform();
         Matrix projMatrix = Matrix::CreatePerspectiveProj(
            m_mainCamera->GetFov(),
            m_window->GetAspectRatio(),
            m_mainCamera->GetNearPlane(),
            m_mainCamera->GetFarPlane());

         m_ssaoPass->UpdateVariableParams(deviceContext,
            {
               projMatrix, 
               m_ssaoRadius, m_ssaoBias, m_ssaoMagnitude
            });
         deviceContext.DrawIndexed(m_screenQuad->GetIndexCount(), 0, 0);

         m_ssaoPass->Unbind(deviceContext);
         return m_ssaoPass->GetOutputBuffer();
      }

      return nullptr;
   }

   RenderTargetDX11* RendererDX11::SSAOBlur(ID3D11DeviceContext& deviceContext, RenderTargetDX11* ssaoInput)
   {
      if (m_ssaoBlurPass->Bind(deviceContext, ssaoInput))
      {
         m_depthDisable->Bind(deviceContext);
         m_viewport->Bind(deviceContext);
         m_defaultRasterizerState->Bind(deviceContext);
         m_screenQuad->Bind(deviceContext, 0);

         deviceContext.DrawIndexed(m_screenQuad->GetIndexCount(), 0, 0);

         m_ssaoBlurPass->Unbind(deviceContext);
         return m_ssaoBlurPass->GetOutputBuffer();
      }

      return nullptr;
   }

   RenderTargetDX11* RendererDX11::ExtractBrightness(ID3D11DeviceContext& deviceContext, GBuffer* gBuffer, RenderTargetDX11* renderBuffer, float depthThreshold, float threshold)
   {
      if (m_extractBrightnessPass->Bind(deviceContext, gBuffer, renderBuffer))
      {
         m_depthDisable->Bind(deviceContext);
         m_viewport->Bind(deviceContext);
         m_defaultRasterizerState->Bind(deviceContext);
         m_screenQuad->Bind(deviceContext, 0);

         m_extractBrightnessPass->UpdateParameters(deviceContext, { threshold });
         deviceContext.DrawIndexed(m_screenQuad->GetIndexCount(), 0, 0);

         m_extractBrightnessPass->Unbind(deviceContext);
         return m_extractBrightnessPass->GetOutputBuffer();
      }

      return renderBuffer;
   }

   RenderTargetDX11* RendererDX11::GaussianBlur(ID3D11DeviceContext& deviceContext, RenderTargetDX11* renderBuffer, unsigned int gaussianAmount)
   {
      if (m_gaussianBlurPass->Bind(deviceContext, renderBuffer))
      {
         m_depthDisable->Bind(deviceContext);
         m_viewport->Bind(deviceContext);
         m_defaultRasterizerState->Bind(deviceContext);
         m_screenQuad->Bind(deviceContext, 0);

         bool bHorizontal = true;
         for (unsigned int epoch = 0; epoch < gaussianAmount; ++epoch)
         {
            if (epoch > 0)
            {
               m_gaussianBlurPass->SwapBuffers(deviceContext, bHorizontal);
            }

            m_gaussianBlurPass->UpdateParameters(deviceContext, { static_cast<unsigned int>( bHorizontal ? 1 : 0 ) });
            deviceContext.DrawIndexed(m_screenQuad->GetIndexCount(), 0, 0);

            bHorizontal = !bHorizontal;
         }

         m_gaussianBlurPass->Unbind(deviceContext);
         return m_gaussianBlurPass->GetOutputBuffer();
      }

      return renderBuffer;
   }

   RenderTargetDX11* RendererDX11::Bloom(ID3D11DeviceContext& deviceContext, RenderTargetDX11* renderBuffer)
   {
      RenderTargetDX11* output = renderBuffer;
      switch (m_bloomType)
      {
      case EBloomType::Box:
         output = BoxBloom(deviceContext, renderBuffer);
         break;

      case EBloomType::Gaussian:
         output = GaussianBloom(deviceContext, renderBuffer);
      case EBloomType::None:
         break;
      }

      return output;
   }

   RenderTargetDX11* RendererDX11::BoxBloom(ID3D11DeviceContext& deviceContext, RenderTargetDX11* renderBuffer)
   {
      if (m_boxBloomPass->Bind(deviceContext, renderBuffer))
      {
         m_boxBloomPass->UpdateParameters(deviceContext, {});
         m_depthDisable->Bind(deviceContext);
         m_viewport->Bind(deviceContext);
         m_defaultRasterizerState->Bind(deviceContext);
         m_screenQuad->Bind(deviceContext, 0);

         deviceContext.DrawIndexed(m_screenQuad->GetIndexCount(), 0, 0);

         m_boxBloomPass->Unbind(deviceContext);
         return Blending(deviceContext, m_boxBloomPass->GetOutputBuffer(), renderBuffer);
      }

      return nullptr;
   }

   RenderTargetDX11* RendererDX11::GaussianBloom(ID3D11DeviceContext& deviceContext, RenderTargetDX11* renderBuffer)
   {
      RenderTargetDX11* extractedBrightness = ExtractBrightness(deviceContext, m_gBuffer, renderBuffer, m_gaussianBloomDepthThreshold, m_gaussianBloomThreshold);
      RenderTargetDX11* blurredExtractedBrightness = GaussianBlur(deviceContext, extractedBrightness, m_gaussianBloomAmount);
      return Blending(deviceContext, blurredExtractedBrightness, renderBuffer, m_gaussianBloomIntensity, 1.0f);
   }

   RenderTargetDX11* RendererDX11::Blending(ID3D11DeviceContext& deviceContext, RenderTargetDX11* srcBuffer, RenderTargetDX11* destBuffer, float srcRatio, float destRatio)
   {
      if (srcBuffer != nullptr && destBuffer != nullptr)
      {
         if (m_blendingPass->Bind(deviceContext, srcBuffer, destBuffer))
         {
            m_blendingPass->UpdateParameters(deviceContext,
               {
                  srcRatio,
                  destRatio
               });
            m_depthDisable->Bind(deviceContext);
            m_viewport->Bind(deviceContext);
            m_defaultRasterizerState->Bind(deviceContext);
            m_screenQuad->Bind(deviceContext, 0);

            deviceContext.DrawIndexed(m_screenQuad->GetIndexCount(), 0, 0);
            m_blendingPass->Unbind(deviceContext);

            return m_blendingPass->GetOutputBuffer();
         }
      }

      return destBuffer;
   }

   void RendererDX11::ToneMappingWithGammaCorrection(ID3D11DeviceContext& deviceContext, RenderTargetDX11* renderBuffer)
   {
      if (m_toneMappingPass->Bind(deviceContext, renderBuffer))
      {
         m_toneMappingPass->UpdateParameters(
            deviceContext,
            {
               m_exposureFactor,
               m_gammaFactor
            });

         m_outputRenderTarget->BindAsRenderTarget(deviceContext, true, false);
         m_depthDisable->Bind(deviceContext);
         m_viewport->Bind(deviceContext);
         m_defaultRasterizerState->Bind(deviceContext);
         m_screenQuad->Bind(deviceContext, 0);

         deviceContext.DrawIndexed(m_screenQuad->GetIndexCount(), 0, 0);
         m_outputRenderTarget->UnbindRenderTarget(deviceContext);
         m_toneMappingPass->Unbind(deviceContext);
      }
   }

   void RendererDX11::Clear(ID3D11DeviceContext& deviceContext)
   {
      float clearColor[4] = { m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w };
      deviceContext.ClearRenderTargetView(m_renderTargetView,
         clearColor);

      ClearDepthStencil(deviceContext);
   }

   void RendererDX11::ClearDepthStencil(ID3D11DeviceContext& deviceContext)
   {
      if (m_bDepthStencilEnabled)
      {
         deviceContext.ClearDepthStencilView(
            m_depthStencilBuffer->GetDSV(),
            D3D11_CLEAR_DEPTH, 1.0f, 0);
      }
   }

   void RendererDX11::Present()
   {
      if (m_swapChain != nullptr)
      {
         /* @TODO: Impl VSYNC **/
         if (m_vsyncEnabled)
         {
            m_swapChain->Present(1, 0);
         }
         else
         {
            m_swapChain->Present(0, 0);
         }
      }
   }

   void RendererDX11::SetClearColor(Vector4 clearColor)
   {
      m_clearColor = clearColor;
   }

   void RendererDX11::SetDepthStencilEnable(ID3D11DeviceContext& deviceContext, bool bDepthStencilEnable)
   {
      m_bDepthStencilEnabled = bDepthStencilEnable;
      SetBackbufferAsRenderTarget(deviceContext);
   }

   void RendererDX11::SetEquirectangularMap(Texture2D* texture)
   {
      if (m_equirectangularMap != texture)
      {
         m_equirectangularMap = texture;
         m_bCubemapDirtyFlag = true;
      }
   }

   void RendererDX11::SetComputeIBLAsRealtime(bool bComputeIBLAsRealtime)
   {
      m_bAlwaysComputeIBL = 
         (m_equirectangularMap != nullptr) ?
         bComputeIBLAsRealtime : false;
   }

   void RendererDX11::SetBackbufferAsRenderTarget(ID3D11DeviceContext& deviceContext)
   {
      if (m_bDepthStencilEnabled)
      {
         deviceContext.OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilBuffer->GetDSV());
      }
      else
      {
         deviceContext.OMSetRenderTargets(1, &m_renderTargetView, nullptr);
      }
   }

   ID3D11DeviceContext* RendererDX11::GetRenderContextByType(ERenderContextType type)
   {
      if (type != ERenderContextType::Immediate)
      {
         return m_deferredContexts[static_cast<unsigned int>(type)];
      }

      return m_immediateContext;
   }
}
