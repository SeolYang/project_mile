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
#include "Math/Vector2.h"
#include "MT/ThreadPool.h"

namespace Mile
{
   RendererDX11::RendererDX11(Context* context) :
      SubSystem(context),
      m_window(nullptr), m_clearColor{ 0.0f, 0.0f, 0.0f, 1.0f },
      m_device(nullptr), m_immediateContext(nullptr), m_deferredContexts{ nullptr },
      m_swapChain(nullptr), m_renderTargetView(nullptr), m_depthStencilBuffer(nullptr), m_bDepthStencilEnabled(true),
      m_gBuffer(nullptr), m_geometryPass(nullptr), m_lightingPass(nullptr),
      m_equirectToCubemapPass(nullptr), m_equirectangularMap(nullptr), m_cubeMesh(nullptr),
      m_bCubemapDirtyFlag(false), m_bAlwaysCalculateDiffuseIrradiance(false),
      m_mainCamera(nullptr), m_viewport(nullptr),
      m_defaultRasterizerState(nullptr), m_additiveBlendState(nullptr), m_defaultBlendState(nullptr),
      m_depthLessEqual(nullptr)
   {
   }

   RendererDX11::~RendererDX11()
   {
      DeInit();
   }

   bool RendererDX11::Init()
   {
      if (m_context == nullptr || m_bIsInitialized)
      {
         MELog(m_context, TEXT("RendererDX11"), ELogType::WARNING, TEXT("RendererDX11 already initialized."), true);
         return false;
      }

      /* Initialize low level rendering api **/
      m_window = m_context->GetSubSystem<Window>();
      if (m_window == nullptr)
      {
         MELog(m_context,
            TEXT("RendererDX11"),
            ELogType::FATAL,
            TEXT("Cannot found Window subsystem from Context."), true);
         return false;
      }

      if (!CreateDeviceAndSwapChain())
      {
         MELog(m_context,
            TEXT("RendererDX11"),
            ELogType::FATAL,
            TEXT("Failed to create Device and SwapChain."), true);
         return false;
      }

      if (!CreateDepthStencilBuffer())
      {
         MELog(m_context,
            TEXT("RendererDX11"),
            ELogType::FATAL,
            TEXT("Failed to create Depth-Stencil Buffer."), true);
         return false;
      }

      m_backBuffer = new RenderTargetDX11(this);
      if (!m_backBuffer->Init(m_renderTargetView, m_depthStencilBuffer))
      {
         return false;
      }

      /* Initialize Pre Light Pass **/
      m_screenQuad = new Quad(this);
      if (!m_screenQuad->Init(-1.0f, -1.0f, 1.0f, 1.0f))
      {
         MELog(m_context,
            TEXT("RendererDX11"),
            ELogType::FATAL,
            TEXT("Failed to create screen quad."), true);
         return false;
      }

      // @TODO: Multiple viewports
      Vector2 screenRes{ m_window->GetResolution() };
      m_viewport = new Viewport(this);
      m_viewport->SetWidth(screenRes.x);
      m_viewport->SetHeight(screenRes.y);

      m_defaultRasterizerState = new RasterizerState(this);
      if (!m_defaultRasterizerState->Init())
      {
         MELog(m_context,
            TEXT("RendererDX11"),
            ELogType::FATAL,
            TEXT("Failed to create Default RasteirzerState."), true);
         return false;
      }

      m_gBuffer = new GBuffer(this);
      if (!m_gBuffer->Init(
         static_cast<unsigned int>(screenRes.x),
         static_cast<unsigned int>(screenRes.y)))
      {
         MELog(m_context,
            TEXT("RendererDX11"),
            ELogType::FATAL,
            TEXT("Failed to create GBuffer."), true);
         return false;
      }
      m_gBuffer->SetDepthStencilBuffer(m_depthStencilBuffer);

      m_geometryPass = new GeometryPass(this);
      if (!m_geometryPass->Init(TEXT("Contents/Shaders/GeometryPass.hlsl")))
      {
         MELog(m_context,
            TEXT("RendererDX11"),
            ELogType::FATAL,
            TEXT("Failed to create Geometry Pass."), true);
         return false;
      }
      m_geometryPass->SetGBuffer(m_gBuffer);

      m_lightingPass = new LightingPass(this);
      if (!m_lightingPass->Init(TEXT("Contents/Shaders/LightingPass.hlsl")))
      {
         MELog(m_context,
            TEXT("RendererDX11"),
            ELogType::FATAL,
            TEXT("Failed to create Lighting Pass."), true);
         return false;
      }
      m_lightingPass->SetGBuffer(m_gBuffer);

      m_defaultBlendState = new BlendState(this);
      if (!m_defaultBlendState->Init())
      {
         return false;
      }

      m_additiveBlendState = new BlendState(this);
      if (!m_additiveBlendState->Init())
      {
         return false;
      }

      m_additiveBlendState->SetRenderTargetBlendState(
         {
            true,
            EBlend::ONE, EBlend::ONE, EBlendOP::ADD,
            EBlend::ONE, EBlend::ZERO, EBlendOP::ADD,
            (UINT8)EColorWriteEnable::ColorWriteEnableAll
         });

      m_equirectToCubemapPass = new Equirect2CubemapPass(this);
      if (!m_equirectToCubemapPass->Init(TEXT("Contents/Shaders/Equirectangular2Cube.hlsl"), DYNAMIC_CUBEMAP_SIZE))
      {
         MELog(m_context,
            TEXT("RendererDX11"),
            ELogType::FATAL,
            TEXT("Failed to create Equirectangular to cubemap convert pass."), true);
         return false;
      }

      m_cubeMesh = new Cube(this);
      if (!m_cubeMesh->Init(Vector3(-1.0f, -1.0f, -1.0f), Vector3(1.0f, 1.0f, 1.0f)))
      {
         MELog(m_context,
            TEXT("RendererDX11"),
            ELogType::FATAL,
            TEXT("Failed to create Cube mesh."), true);
         return false;
      }

      m_depthLessEqual = new DepthStencilState(this);
      D3D11_DEPTH_STENCIL_DESC depthLessEqualDesc = DepthStencilState::GetDefaultDesc();
      depthLessEqualDesc.DepthEnable = true;
      depthLessEqualDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
      m_depthLessEqual->SetDesc(depthLessEqualDesc);
      if (!m_depthLessEqual->Init())
      {
         MELog(m_context,
            TEXT("RendererDX11"),
            ELogType::FATAL,
            TEXT("Failed to create DEPTH LESS EQUAL state."), true);
         return false;
      }

      MELog(m_context, TEXT("RendererDX11"), ELogType::MESSAGE, TEXT("RendererDX11 Initialized!"), true);
      m_bIsInitialized = true;
      return true;
   }

   void RendererDX11::DeInit()
   {
      if (m_bIsInitialized)
      {
         SafeDelete(m_depthLessEqual);
         SafeDelete(m_viewport);
         SafeDelete(m_additiveBlendState);
         SafeDelete(m_defaultBlendState);
         SafeDelete(m_defaultRasterizerState);
         SafeDelete(m_screenQuad);
         SafeDelete(m_cubeMesh);
         SafeDelete(m_geometryPass);
         SafeDelete(m_lightingPass);
         SafeDelete(m_gBuffer);
         SafeRelease(m_renderTargetView);
         SafeDelete(m_depthStencilBuffer);
         SafeDelete(m_equirectToCubemapPass);
         SafeRelease(m_swapChain);

         for (auto deferredContext : m_deferredContexts)
         {
            SafeRelease(deferredContext);
         }

         SafeRelease(m_immediateContext);
         SafeRelease(m_device);

         SubSystem::DeInit();
         MELog(m_context, TEXT("RendererDX11"), ELogType::MESSAGE, TEXT("RendererDX11 deinitialized."), true);
      }
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

         m_meshRenderComponents = std::move(world->GetComponentsFromEntities<MeshRenderComponent>(true, true));
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
         m_lightComponents = std::move(world->GetComponentsFromEntities<LightComponent>(true, true));
      }
   }

   void RendererDX11::AcquireCameras(World* world)
   {
      m_cameras.clear();
      if (world != nullptr)
      {
         m_cameras = std::move(world->GetComponentsFromEntities<CameraComponent>(true, true));
      }
   }

   void RendererDX11::Render()
   {
      auto threadPool = m_context->GetSubSystem<ThreadPool>();
      //Clear( *m_immediateContext );

      World* world = m_context->GetSubSystem<World>();
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

         auto calcDiffuseIrradiancePassBind = std::bind(
            &RendererDX11::CalculateDiffuseIrradiance,
            this,
            GetRenderContextByType(ERenderContextType::PreProcess));
         auto calcDiffuseIrradiancePassTask = threadPool->AddTask(calcDiffuseIrradiancePassBind);

         acquireMeshRenderersAndMatTask.get();
         acquireLightTask.get();
         acquireCamerasTask.get();

         // @TODO: Implement Multiple camera rendering
         if (!m_cameras.empty())
         {
            m_mainCamera = m_cameras[0];
            SetClearColor(m_mainCamera->GetClearColor());

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

            ID3D11CommandList* calcDiffuseIrraidnaceCmdList = calcDiffuseIrradiancePassTask.get();
            ID3D11CommandList* geometryPassCmdList = geometryPassTask.get();
            ID3D11CommandList* lightingPassCmdList = lightingPassTask.get();

            SAFE_EXECUTE_CMDLIST(m_immediateContext, calcDiffuseIrraidnaceCmdList, false);
            SAFE_EXECUTE_CMDLIST(m_immediateContext, geometryPassCmdList, false);
            SAFE_EXECUTE_CMDLIST(m_immediateContext, lightingPassCmdList, false);

            SafeRelease(calcDiffuseIrraidnaceCmdList);
            SafeRelease(geometryPassCmdList);
            SafeRelease(lightingPassCmdList);
         }
      }

      Present();
   }

   ID3D11CommandList* RendererDX11::CalculateDiffuseIrradiance(ID3D11DeviceContext* deviceContextPtr)
   {
      if (m_bCubemapDirtyFlag || m_bAlwaysCalculateDiffuseIrradiance)
      {
         if (deviceContextPtr != nullptr)
         {
            ID3D11DeviceContext& deviceContext = (*deviceContextPtr);
            deviceContext.ClearState();
            deviceContext.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            ConvertEquirectToCubemap(deviceContext);
            SolveDiffuseIntegral(deviceContext);

            ID3D11CommandList* commandList = nullptr;
            auto result = deviceContext.FinishCommandList(false, &commandList);
            if (!FAILED(result))
            {
               m_bCubemapDirtyFlag = false;
               return commandList;
            }
         }
      }

      return nullptr;
   }

   void RendererDX11::ConvertEquirectToCubemap(ID3D11DeviceContext& deviceContext)
   {
      Matrix captureProj = Matrix::CreatePerspectiveProj(90.0f, 1.0f, 0.1f, 1000.0f);
      Matrix captureMatrix[] =
      {
         // +x
         Matrix::CreateView(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f)) * captureProj,
         // -x
         Matrix::CreateView(Vector3(0.0f, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f)) * captureProj,
         // +y
         Matrix::CreateView(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f)) * captureProj,
         // -y
         Matrix::CreateView(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f)) * captureProj,
         // +z
         Matrix::CreateView(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, -1.0f, 0.0f)) * captureProj,
         // -z
         Matrix::CreateView(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, -1.0f, 0.0f)) * captureProj
      };

      DynamicCubemap* cubemap = m_equirectToCubemapPass->GetCubemap();
      m_equirectToCubemapPass->Bind(deviceContext, m_equirectangularMap->GetRawTexture(), 0);
      m_depthLessEqual->Bind(deviceContext);
      for (unsigned int faceIndex = 0; faceIndex < CUBE_FACES; ++faceIndex)
      {
         cubemap->BindAsRenderTarget(deviceContext, faceIndex);
         m_equirectToCubemapPass->UpdateTransformBuffer(
            deviceContext,
            { captureMatrix[faceIndex] });
         m_cubeMesh->Bind(deviceContext, 0);
         deviceContext.DrawIndexed(m_cubeMesh->GetIndexCount(), 0, 0);
      }

      m_equirectToCubemapPass->Unbind(deviceContext);
      cubemap->GenerateMips(deviceContext);
   }

   void RendererDX11::SolveDiffuseIntegral(ID3D11DeviceContext& deviceContext)
   {
   }

   ID3D11CommandList* RendererDX11::RunGeometryPass(ID3D11DeviceContext* deviceContextPtr)
   {
      if (deviceContextPtr != nullptr)
      {
         ID3D11DeviceContext& deviceContext = *deviceContextPtr;
         deviceContext.ClearState();
         deviceContext.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // @TODO: Input Assembly 세팅을 Rendering Pass 안으로
         m_defaultRasterizerState->Bind(deviceContext);
         m_viewport->Bind(deviceContext);
         m_geometryPass->Bind(deviceContext);

         auto camTransform = m_mainCamera->GetTransform();
         for (auto batchedMaterial : m_materialMap)
         {
            /* 물리 기반 머테리얼 정보 취득 **/
            Material* material = batchedMaterial.first;
            Texture2dDX11* baseColorTex = material->GetTexture2D(MaterialTextureProperty::BaseColor)->GetRawTexture();
            Texture2dDX11* emissiveTex = material->GetTexture2D(MaterialTextureProperty::Emissive)->GetRawTexture();
            Texture2dDX11* metallicRoughnessTex = material->GetTexture2D(MaterialTextureProperty::MetallicRoughness)->GetRawTexture();
            Texture2dDX11* aoTex = material->GetTexture2D(MaterialTextureProperty::AO)->GetRawTexture();
            Texture2dDX11* normalTex = material->GetTexture2D(MaterialTextureProperty::Normal)->GetRawTexture();

            Vector4 baseColorFactor = material->GetVector4Factor(MaterialFactorProperty::BaseColor);
            Vector4 emissiveFactor = material->GetVector4Factor(MaterialFactorProperty::Emissive);
            float metallicFactor = material->GetScalarFactor(MaterialFactorProperty::Metallic);
            float roughnessFactor = material->GetScalarFactor(MaterialFactorProperty::Roughness);

            m_geometryPass->UpdateMaterialBuffer(
               deviceContext,
               { baseColorFactor, emissiveFactor, metallicFactor, roughnessFactor });

            SAFE_TEX_BIND(baseColorTex, deviceContext, 0, EShaderType::PixelShader);
            SAFE_TEX_BIND(emissiveTex, deviceContext, 1, EShaderType::PixelShader);
            SAFE_TEX_BIND(metallicRoughnessTex, deviceContext, 2, EShaderType::PixelShader);
            SAFE_TEX_BIND(aoTex, deviceContext, 3, EShaderType::PixelShader);
            SAFE_TEX_BIND(normalTex, deviceContext, 4, EShaderType::PixelShader);

            for (auto meshRenderer : batchedMaterial.second)
            {
               Transform* transform = meshRenderer->GetTransform();
               Mesh* mesh = meshRenderer->GetMesh();

               Matrix worldMatrix = transform->GetWorldMatrix();
               Matrix viewMatrix = Matrix::CreateView(
                  camTransform->GetPosition(TransformSpace::World),
                  camTransform->GetForward(TransformSpace::World),
                  camTransform->GetUp(TransformSpace::World));
               Matrix projMatrix = Matrix::CreatePerspectiveProj(
                  m_mainCamera->GetFov(),
                  m_window->GetAspectRatio(),
                  m_mainCamera->GetNearPlane(),
                  m_mainCamera->GetFarPlane());

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
            SAFE_TEX_UNBIND(aoTex, deviceContext);
            SAFE_TEX_UNBIND(normalTex, deviceContext);
         }

         m_geometryPass->Unbind(deviceContext);

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
         SetDepthStencilEnable(deviceContext, false);
         deviceContext.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // @TODO: Input Assembly 세팅을 Rendering Pass 안으로
         m_lightingPass->Bind(deviceContext);
         m_viewport->Bind(deviceContext);
         m_defaultRasterizerState->Bind(deviceContext);
         m_backBuffer->BindAsRenderTarget(deviceContext);
         m_screenQuad->Bind(deviceContext, 0);
         m_additiveBlendState->Bind(deviceContext);

         for (auto lightComponent : m_lightComponents)
         {
            Transform* camTransform = m_mainCamera->GetTransform();
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

            m_backBuffer->ClearDepthStencil(deviceContext);
            deviceContext.DrawIndexed(m_screenQuad->GetIndexCount(), 0, 0);
         }

         SetDepthStencilEnable(deviceContext, true);
         m_lightingPass->Unbind(deviceContext);
         m_backBuffer->UnbindRenderTarget(deviceContext);

         // @TODO: Emmisive post processing 단으로 따로 빼내기

         ID3D11CommandList* commandList = nullptr;
         auto result = deviceContext.FinishCommandList(false, &commandList);
         if (!FAILED(result))
         {
            return commandList;
         }
      }

      return nullptr;
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
         m_swapChain->Present(0, 0);
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

   void RendererDX11::SetAlwaysCalculateDiffuseIrradiacne(bool bAlwaysCalculateDiffuseIrraidiance)
   {
      m_bAlwaysCalculateDiffuseIrradiance = 
         (m_equirectangularMap != nullptr) ? 
         bAlwaysCalculateDiffuseIrraidiance : false;
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
