#include "RendererDX11.h"
#include "DepthStencilBufferDX11.h"
#include "RenderTargetDX11.h"
#include "Mesh.h"
#include "Quad.h"
#include "GBuffer.h"
#include "GBufferPass.h"
#include "LightBufferPass.h"
#include "ShadingPass.h"
#include "Core\Context.h"
#include "Core\Window.h"
#include "Core\World.h"
#include "Core\Entity.h"
#include "Core\Transform.h"
#include "Math\Vector2.h"
#include "Component\MeshRenderComponent.h"
#include "Component\LightComponent.h"
#include "Component\CameraComponent.h"
#include "Resource\Material.h"
#include "Resource\Texture2D.h"

namespace Mile
{
   RendererDX11::RendererDX11( Context* context ) : SubSystem( context ),
      m_window( nullptr ),
      m_device( nullptr ), m_deviceContext( nullptr ),
      m_swapChain( nullptr ), m_renderTargetView( nullptr ),
      m_depthStencilBuffer( nullptr ), m_bDepthStencilEnabled( true ),
      m_gBuffer( nullptr ), m_gBufferPass( nullptr ),
      m_lightBuffer( nullptr ), m_lightBufferPass( nullptr ),
      m_shadingPass( nullptr ),
      m_mainCamera( nullptr )
   {
   }

   RendererDX11::~RendererDX11( )
   {
      SafeDelete( m_screenQuad );
      SafeDelete( m_shadingPass );
      SafeDelete( m_lightBufferPass );
      SafeDelete( m_lightBuffer );
      SafeDelete( m_gBufferPass );
      SafeDelete( m_gBuffer );
      SafeRelease( m_renderTargetView );
      SafeDelete( m_depthStencilBuffer );
      SafeRelease( m_swapChain );
      SafeRelease( m_deviceContext );
      SafeRelease( m_device );
   }

   bool RendererDX11::Init( )
   {
      // Initialize low level systems
      m_window = m_context->GetSubSystem<Window>( );
      if ( !CreateDeviceAndSwapChain( ) )
      {
         return false;
      }

      if ( !CreateDepthStencilBuffer( ) )
      {
         return false;
      }
      // #Initialize low level systems

      // Initialize Pre Light pass
      Vector2 screenRes{ m_window->GetResolution( ) };

      m_screenQuad = new Quad( this );
      if ( !m_screenQuad->Init( 0.0f, 0.0f, screenRes.x, screenRes.y ) )
      {
         return false;
      }

      m_gBuffer = new GBuffer( this );
      if ( !m_gBuffer->Init(
         static_cast< unsigned int >( screenRes.x ),
         static_cast< unsigned int >( screenRes.y ) ) )
      {
         return false;
      }
      m_gBuffer->SetDepthStencilBuffer( m_depthStencilBuffer );

      m_gBufferPass = new GBufferPass( this );
      if ( !m_gBufferPass->Init( TEXT( "Shaders/GBuffer.hlsl" ) ) )
      {
         return false;
      }
      m_gBufferPass->SetGBuffer( m_gBuffer );

      m_lightBuffer = new RenderTargetDX11( this );
      if ( !m_lightBuffer->Init(
         static_cast< unsigned int >( screenRes.x ),
         static_cast< unsigned int >( screenRes.y ) ) )
      {
         return false;
      }

      m_lightBufferPass = new LightBufferPass( this );
      if ( !m_lightBufferPass->Init( TEXT( "Shaders/LightBuffer.hlsl" ) ) )
      {
         return false;
      }
      m_lightBufferPass->SetGBuffer( m_gBuffer );
      m_lightBufferPass->SetLightBuffer( m_lightBuffer );

      m_shadingPass = new ShadingPass( this );
      if ( !m_shadingPass->Init( TEXT( "Shaders/Shading.hlsl" ) ) )
      {
         return false;
      }
      m_shadingPass->SetLightBuffer( m_lightBuffer );
      m_shadingPass->AcquireTransformBuffer( m_gBufferPass );
      // #Initialize Pre Light pass

      return true;
   }

   bool RendererDX11::CreateDeviceAndSwapChain( )
   {
      DXGI_SWAP_CHAIN_DESC swDesc;
      ZeroMemory( &swDesc, sizeof( swDesc ) );

      swDesc.BufferCount = 1;
      swDesc.BufferDesc.Width = m_window->GetResWidth( );
      swDesc.BufferDesc.Height = m_window->GetResHeight( );
      swDesc.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
      swDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
      swDesc.OutputWindow = reinterpret_cast< HWND >( m_window->GetHandle( ) );
      swDesc.SampleDesc.Count = 1;
      swDesc.SampleDesc.Quality = 0;
      swDesc.Windowed = !m_window->IsFullScreen( );

      unsigned int createDeviceFlags = 0;
#ifdef _DEBUG
      createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

      D3D_FEATURE_LEVEL featureLevels[ ] = { D3D_FEATURE_LEVEL_11_0 };
      unsigned int numOfFeatureLevels = ARRAYSIZE( featureLevels );
      D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;


      auto hr = D3D11CreateDeviceAndSwapChain( nullptr,
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
                                               &m_deviceContext );

      if ( FAILED( hr ) )
      {
         /* Failed to create device and swap chain. **/
         return false;
      }

      /* Create Render target view **/
      ID3D11Texture2D* backBuffer = nullptr;
      hr = m_swapChain->GetBuffer( 0,
                                   __uuidof( ID3D11Texture2D ),
                                   ( LPVOID* ) &backBuffer );

      if ( FAILED( hr ) )
      {
         /* Failed to get back buffer texture resource from swap chain. **/
         return false;
      }

      hr = m_device->CreateRenderTargetView( backBuffer,
                                             nullptr,
                                             &m_renderTargetView );
      SafeRelease( backBuffer );

      if ( FAILED( hr ) )
      {
         /* Failed to create render target view with backbuffer. **/
         return false;
      }

      return true;
   }

   bool RendererDX11::CreateDepthStencilBuffer( )
   {
      if ( m_device == nullptr )
      {
         return false;
      }

      m_depthStencilBuffer = new DepthStencilBufferDX11( this );
      bool res = m_depthStencilBuffer->Init( m_window->GetResWidth( ),
                                             m_window->GetResHeight( ),
                                             true );

      if ( !res )
      {
         return false;
      }

      return true;
   }

   void RendererDX11::AcquireMeshRenderersAndMaterial( const std::vector<Entity*>& entities )
   {
      m_meshRenderComponents.clear( );
      m_materialMap.clear( );
      for ( auto entity : entities )
      {
         auto meshRenderComponent = entity->GetComponent<MeshRenderComponent>( );
         if ( meshRenderComponent != nullptr )
         {
            if ( entity->IsActive( ) && meshRenderComponent->IsActive( ) )
            {
               auto material = meshRenderComponent->GetMaterial( );
               if ( material != nullptr )
               {
                  // Material Batching
                  m_meshRenderComponents.push_back( meshRenderComponent );
                  m_materialMap[ material ].push_back( meshRenderComponent );
               }
            }
         }
      }
   }

   void RendererDX11::AcquireLights( const std::vector<Entity*>& entities )
   {
      m_lightComponents.clear( );
      for ( auto entity : entities )
      {
         auto lightComponent = entity->GetComponent<LightComponent>( );
         if ( lightComponent != nullptr )
         {
            if ( entity->IsActive( ) && lightComponent->IsActive( ) )
            {
               m_lightComponents.push_back( lightComponent );
            }
         }
      }
   }

   void RendererDX11::AcquireCameras( const std::vector<Entity*>& entities )
   {
      m_cameras.clear( );
      for ( auto entity : entities )
      {
         auto cameraComponent = entity->GetComponent<CameraComponent>( );
         if ( cameraComponent != nullptr )
         {
            if ( entity->IsActive( ) && cameraComponent->IsActive( ) )
            {
               m_cameras.push_back( cameraComponent );
            }
         }
      }
   }

   void RendererDX11::Render( )
   {
      Clear( );

      World* world = m_context->GetSubSystem<World>( );
      if ( world != nullptr )
      {
         auto entities = world->GetEntities( );

         // @TODO: Multi-threaded resource acquire
         // Acquire necessarry informations
         AcquireMeshRenderersAndMaterial( entities );
         AcquireLights( entities );
         AcquireCameras( entities );

         if ( !m_cameras.empty( ) )
         {
            // @TODO: Implement Multiple camera rendering
            m_mainCamera = m_cameras[ 0 ];


            // @TODO: Set Viewport

            // light pre pass rendering
            RenderGBuffer( );
            RenderLightBuffer( );
            RenderShading( );
         }
      }

      Present( );
   }

   void RendererDX11::RenderGBuffer( )
   {
      m_gBuffer->SetDepthStencilBuffer( m_depthStencilBuffer );
      m_gBufferPass->Bind( );

      auto camTransform = m_mainCamera->GetTransform( );

      for ( auto batchedMaterial : m_materialMap )
      {
         auto material = batchedMaterial.first;
         auto normalTexture = material->GetNormalMap( );
         m_gBufferPass->UpdateMaterialBuffer( material->GetSpecularExp( ) ); // per material
         m_gBufferPass->UpdateNormalTexture( normalTexture->GetRawTexture( ) ); // per material

         for ( auto meshRenderer : batchedMaterial.second )
         {
            auto transform = meshRenderer->GetEntity( )->GetTransform( );
            auto mesh = meshRenderer->GetMesh( );
            
            Matrix world = transform->GetWorldMatrix( );
            Matrix worldView = world * 
               Matrix::CreateView( 
                  camTransform->GetPosition( TransformSpace::World ),
                  camTransform->GetForward( ) );
            Matrix worldViewProj = worldView *
               Matrix::CreatePerspectiveProj(
                  m_mainCamera->GetFov( ),
                  m_window->GetAspectRatio( ),
                  m_mainCamera->GetNearPlane( ),
                  m_mainCamera->GetFarPlane( ) );

            m_gBufferPass->UpdateTransformBuffer( world, worldView, worldViewProj );  // per object
            mesh->Bind( 0 );

            // @TODO: Implement instancing
            m_deviceContext->DrawIndexed( mesh->GetIndexCount( ), 0, 0 );
         }

         // Unbind shader resource
         m_gBufferPass->UpdateNormalTexture( nullptr );
      }

      // End of gbuffer pass
      m_gBufferPass->Unbind( );
   }

   void RendererDX11::RenderLightBuffer( )
   {
      m_lightBufferPass->SetGBuffer( m_gBuffer );
      m_lightBufferPass->SetLightBuffer( m_lightBuffer );
      m_lightBufferPass->Bind( );

      // @TODO: Implement multi - camera rendering
      auto cameraTransform = m_cameras[ 0 ]->GetTransform( );
      m_lightBufferPass->UpdateCameraBuffer( cameraTransform->GetPosition( ) );

      for ( auto light : m_lightComponents )
      {
         // @TODO: Add light type
         m_lightBufferPass->UpdateLightParamBuffer(
            light->GetLightPosition( ),
            light->GetLightColor( ),
            light->GetLightDirection( ),
            light->GetSpotlightAngles( ),
            Vector3( light->GetLightRange( ), 0.0f, 0.0f ),
            LightComponent::LightTypeToIndex( light->GetLightType( ) ) );

         m_screenQuad->Bind( 0 );
         m_deviceContext->DrawIndexed( 4, 0, 0 );
      }

      m_lightBufferPass->Unbind( );
   }

   void RendererDX11::RenderShading( )
   {
      m_shadingPass->SetLightBuffer( m_lightBuffer );
      m_shadingPass->AcquireTransformBuffer( m_gBufferPass );
      m_shadingPass->Bind( );
      SetBackbufferAsRenderTarget( );
      ClearDepthStencil( );

      auto camTransform = m_mainCamera->GetTransform( );

      for ( auto batchedMaterial : m_materialMap )
      {
         auto material = batchedMaterial.first;
         auto diffuseTexture = material->GetDiffuseMap( );
         m_shadingPass->UpdateMaterialBuffer( material->GetSpecularAlbedo( ) );
         m_shadingPass->UpdateDiffuseTexture( diffuseTexture->GetRawTexture( ) );

         for ( auto meshRenderer : batchedMaterial.second )
         {
            auto transform = meshRenderer->GetTransform( );
            auto mesh = meshRenderer->GetMesh( );

            Matrix world = transform->GetWorldMatrix( );
            Matrix worldView = world *
               Matrix::CreateView(
                  camTransform->GetPosition( TransformSpace::World ),
                  camTransform->GetForward( ) );
            Matrix worldViewProj = worldView *
               Matrix::CreatePerspectiveProj(
                  m_mainCamera->GetFov( ),
                  m_window->GetAspectRatio( ),
                  m_mainCamera->GetNearPlane( ),
                  m_mainCamera->GetFarPlane( ) );
            // @TODO: add camera
            m_shadingPass->UpdateTransformBuffer( world, worldView, worldViewProj );  // per object
            mesh->Bind( 0 );

            // @TODO: Implement instancing
            m_deviceContext->DrawIndexed( mesh->GetIndexCount( ), 0, 0 );
         }

         // Unbind ShaderResource
         m_shadingPass->UpdateDiffuseTexture( nullptr );
      }

      m_shadingPass->Unbind( );
   }

   void RendererDX11::Clear( )
   {
      if ( m_deviceContext != nullptr )
      {
         float clearColor[ 4 ] = { m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w };
         m_deviceContext->ClearRenderTargetView( m_renderTargetView,
                                                 clearColor );

         ClearDepthStencil( );
      }
   }

   void RendererDX11::ClearDepthStencil( )
   {
      if ( m_bDepthStencilEnabled )
      {
         m_deviceContext->ClearDepthStencilView(
            m_depthStencilBuffer->GetDSV( ),
            D3D11_CLEAR_DEPTH, 1.0f, 0 );
      }
   }

   void RendererDX11::Present( )
   {
      if ( m_swapChain != nullptr )
      {
         /* @TODO: Impl VSYNC **/
         m_swapChain->Present( 0, 0 );
      }
   }

   void RendererDX11::SetClearColor( Vector4 clearColor )
   {
      m_clearColor = clearColor;
   }

   void RendererDX11::SetDepthStencilEnable( bool bDepthStencilEnable )
   {
      m_bDepthStencilEnabled = bDepthStencilEnable;
      SetBackbufferAsRenderTarget( );
   }

   void RendererDX11::SetBackbufferAsRenderTarget( )
   {
      if ( m_bDepthStencilEnabled )
      {
         m_deviceContext->OMSetRenderTargets( 1, &m_renderTargetView, m_depthStencilBuffer->GetDSV( ) );
      }
      else
      {
         m_deviceContext->OMSetRenderTargets( 1, &m_renderTargetView, nullptr );
      }
   }
}