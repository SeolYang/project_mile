#pragma once

#include "Rendering.h"
#include "Viewport.h"

namespace Mile
{
   enum class ShaderType
   {
      VertexShader,
      HullShader,
      DomainShader,
      GeometryShader,
      PixelShader
   };

   class DepthStencilBufferDX11;
   class RenderTargetDX11;
   class Quad;
   class GBuffer;
   class GBufferPass;
   class LightBufferPass;
   class ShadingPass;
   class TestRenderPass;
   class Window;
   class World;
   class Entity;
   class MeshRenderComponent;
   class LightComponent;
   class CameraComponent;
   class Material;
   class RasterizerState;
   class MEAPI RendererDX11 : public SubSystem
   {
      using MaterialMap = std::map<Material*, std::vector<MeshRenderComponent*>>;
   public:
      RendererDX11( Context* context );
      virtual ~RendererDX11( );

      virtual bool Init( ) override;
      virtual void DeInit( ) override;

      void AcquireMeshRenderersAndMaterial( const std::vector<Entity*>& entities );
      void AcquireLights( const std::vector<Entity*>& entities );
      void AcquireCameras( const std::vector<Entity*>& entities );

      void Render( );
      void RenderGBuffer( );
      void RenderLightBuffer( );
      void RenderShading( );
      void RenderTest( );

      void Clear( );
      void ClearDepthStencil( );
      void Present( );

      ID3D11Device* GetDevice( ) { return m_device; }
      ID3D11DeviceContext* GetDeviceContext( ) { return m_deviceContext; }

      void SetClearColor( Vector4 clearColor );
      Vector4 GetClearColor( ) const { return m_clearColor; }

      void SetDepthStencilEnable( bool bDepthEnabled );
      bool IsDepthStencilEnabled( ) const { return m_bDepthStencilEnabled; }

      void SetBackbufferAsRenderTarget( );

   private:
      bool CreateDeviceAndSwapChain( );
      bool CreateDepthStencilBuffer( );

   private:
      Window*                 m_window;
      ID3D11Device*           m_device;
      ID3D11DeviceContext*    m_deviceContext;

      // Back Buffer Variables
      IDXGISwapChain*           m_swapChain;
      ID3D11RenderTargetView*   m_renderTargetView;
      DepthStencilBufferDX11*   m_depthStencilBuffer;
      // \Back Buffer Variables

      // Pre light pass Rendering
      GBuffer*          m_gBuffer;
      GBufferPass*      m_gBufferPass;
      RenderTargetDX11* m_lightBuffer;
      LightBufferPass*  m_lightBufferPass;
      ShadingPass*      m_shadingPass;
      Quad*             m_screenQuad;
      // \Pre light pass Rendering

      // Test Rendering Pass
      TestRenderPass*   m_testPass;
      // \Test Rendering Pass

      // Renderable objects
      std::vector<MeshRenderComponent*> m_meshRenderComponents;
      std::vector<LightComponent*>      m_lightComponents;
      MaterialMap                       m_materialMap;
      std::vector<CameraComponent*>     m_cameras;

      CameraComponent*                  m_mainCamera;
      // \Renderable objects

      bool      m_bDepthStencilEnabled;
      Vector4   m_clearColor;

      // @TODO: Multiple Viewports
      Viewport*  m_viewport;

      // Rasterizer State
      RasterizerState*   m_defaultState;

   };
}