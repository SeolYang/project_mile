#include "Rendering/RendererDX11.h"
#include "Rendering/RenderTargetDX11.h"
#include "Rendering/DepthStencilBufferDX11.h"
#include "Core/Window.h"

namespace Mile
{
   DEFINE_LOG_CATEGORY(MileRenderer);

   RendererDX11::RendererDX11(Context* context, size_t maximumThreads) :
      SubSystem(context),
      m_maximumThreads(maximumThreads),
      m_device(nullptr),
      m_immediateContext(nullptr),
      m_swapChain(nullptr),
      m_backBuffer(nullptr),
      m_backBufferDepthStencil(nullptr),
      m_renderResolution(Vector2(1920.0f, 1080.0f)),
      m_onWindowResize(nullptr),
      m_bVsyncEnabled(false)
   {
   }

   RendererDX11::~RendererDX11()
   {
      SafeDelete(m_onWindowResize);
      SafeDelete(m_backBufferDepthStencil);
      SafeDelete(m_backBuffer);
      SafeRelease(m_swapChain);

      for (auto*& deferredContext : m_deferredContexts)
      {
         SafeRelease(deferredContext);
      }

      SafeRelease(m_immediateContext);
      SafeRelease(m_device);
   }

   bool RendererDX11::Init(Window& window)
   {
      if (SubSystem::Init())
      {
         if (InitLowLevelAPI(window))
         {
            return true;
         }
         else
         {
            ME_LOG(MileRenderer, Fatal, TEXT("Failed to initialize low level graphics API!"));
         }
      }

      ME_LOG(MileRenderer, Fatal, TEXT("Failed to initialize Renderer!"));
      return false;
   }

   bool RendererDX11::InitLowLevelAPI(Window& window)
   {
      DXGI_SWAP_CHAIN_DESC swDesc;
      ZeroMemory(&swDesc, sizeof(swDesc));

      Vector2 clientAreaRes = window.GetResolution();
      swDesc.BufferCount = 1;
      swDesc.BufferDesc.Width = (unsigned int)clientAreaRes.x;
      swDesc.BufferDesc.Height = (unsigned int)clientAreaRes.y;
      swDesc.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
      swDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
      swDesc.OutputWindow = reinterpret_cast<HWND>(window.GetHandle());
      swDesc.SampleDesc.Count = 1;
      swDesc.SampleDesc.Quality = 0;
      swDesc.Windowed = !window.IsFullScreen();

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

      ID3D11RenderTargetView* backBufferRenderTargetView = nullptr;
      hr = m_device->CreateRenderTargetView(
         backBuffer,
         nullptr,
         &backBufferRenderTargetView);
      SafeRelease(backBuffer);

      if (FAILED(hr))
      {
         ME_LOG(MileRenderer, Fatal, TEXT("Failed to create back buffer render target view!"));
         return false;
      }

      /** Create Depth-stencil buffer for Backbuffer. */
      m_backBufferDepthStencil = new DepthStencilBufferDX11(this);
      if (!m_backBufferDepthStencil->Init(clientAreaRes.x, clientAreaRes.y, true))
      {
         ME_LOG(MileRenderer, Fatal, TEXT("Failed to create back buffer depth-stencil buffer!"));
         return false;
      }

      m_backBuffer = new RenderTargetDX11(this);
      if (!m_backBuffer->Init(backBufferRenderTargetView, m_backBufferDepthStencil))
      {
         ME_LOG(MileRenderer, Fatal, TEXT("Failed to create back buffer render target!"));
         return false;
      }

      // Create Deferred Contexts
      for (auto idx = 0; idx < m_maximumThreads; ++idx)
      {
         ID3D11DeviceContext* deferredContext = nullptr;
         hr = m_device->CreateDeferredContext(0, &deferredContext);
         if (FAILED(hr))
         {
            // Failed to create deferred contexts.
            ME_LOG(MileRenderer, Fatal, TEXT("Failed to create deferred context!"));
            return false;
         }

         m_deferredContexts.push_back(deferredContext);
      }

      /** Initialize Delegates */
      m_onWindowResize = new OnWindowResizeDelegate();
      m_onWindowResize->Bind(&RendererDX11::OnWindowReisze, this);
      window.OnWindowResize.Add(m_onWindowResize);

      return true;
   }

   void RendererDX11::Render(const World& world)
   {
      OPTICK_EVENT();
      RenderImpl(world);
   }

   void RendererDX11::Present()
   {
      OPTICK_EVENT();
      if (m_swapChain != nullptr)
      {
         if (m_bVsyncEnabled)
         {
            m_swapChain->Present(1, 0);
         }
         else
         {
            m_swapChain->Present(0, 0);
         }
      }
   }

   void RendererDX11::OnWindowReisze(unsigned int width, unsigned int height)
   {
      SafeDelete(m_backBufferDepthStencil);
      SafeDelete(m_backBuffer);

      m_swapChain->ResizeBuffers(1, 0, 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
      HRESULT hr;
      ID3D11Texture2D* backBuffer = nullptr;
      hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
      if (FAILED(hr))
      {
         ME_LOG(MileRenderer, Fatal, TEXT("Failed to get resized back buffer!"));
         return;
      }

      ID3D11RenderTargetView* backBufferRenderTargetView = nullptr;
      hr = m_device->CreateRenderTargetView(
         backBuffer, nullptr,
         &backBufferRenderTargetView);
      SafeRelease(backBuffer);

      if (FAILED(hr))
      {
         ME_LOG(MileRenderer, Fatal, TEXT("Failed to create resized back buffer render target view!"));
         return;
      }

      m_backBufferDepthStencil = new DepthStencilBufferDX11(this);
      if (!m_backBufferDepthStencil->Init(width, height, true))
      {
         ME_LOG(MileRenderer, Fatal, TEXT("Failed to create resized back buffer depth stencill buffer!"));
         return;
      }

      m_backBuffer = new RenderTargetDX11(this);
      if (!m_backBuffer->Init(backBufferRenderTargetView, m_backBufferDepthStencil))
      {
         ME_LOG(MileRenderer, Fatal, TEXT("Failed to create resized back buffer render target!"));
      }

      ME_LOG(MileRenderer, Log, TEXT("Back buffer successfully resized to (%d x %d)"), width, height);
   }

   void RendererDX11::SetBackBufferAsRenderTarget(ID3D11DeviceContext& deviceContext)
   {
      m_backBuffer->BindAsRenderTarget(deviceContext, true, true);
   }
}