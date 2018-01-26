#pragma once

#include "Rendering.h"
#include "Viewport.h"
#include <array>

namespace Mile
{
   enum class ShaderType
   {
      VertexShader,
      HullShader,
      DomainShader,
      GeometryShader,
      PixelShader,
      EnumSize
   };

   enum class EDeviceContextType : uint32_t
   {
      GBufferPass,
      LBufferPass, // Light Buffer Pass
      ShadingPass,
      Immediate,
      EnumSize = 4
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
   class CommandListDX11;
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
      void RenderLightPrePass( );
      ID3D11CommandList* RenderGBuffer( ID3D11DeviceContext* deviceContext );
      ID3D11CommandList* RenderLightBuffer( ID3D11DeviceContext* deviceContext );
      ID3D11CommandList* RenderShading( ID3D11DeviceContext* deviceContext );
      void RenderTest( ID3D11DeviceContext& deviceContext );

      void Clear( ID3D11DeviceContext& deviceContext );
      void ClearDepthStencil( ID3D11DeviceContext& deviceContext );
      void Present( );

      ID3D11Device* GetDevice( ) { return m_device; }
      ID3D11DeviceContext* GetImmediateContext( ) { return m_immediateContext; }

      void SetClearColor( Vector4 clearColor );
      Vector4 GetClearColor( ) const { return m_clearColor; }

      void SetDepthStencilEnable( ID3D11DeviceContext& deviceContext, bool bDepthEnabled );
      bool IsDepthStencilEnabled( ) const { return m_bDepthStencilEnabled; }

      void SetBackbufferAsRenderTarget(ID3D11DeviceContext& deviceContext );

      ID3D11DeviceContext* GetDeviceContextByType( EDeviceContextType type );

   private:
      bool CreateDeviceAndSwapChain( );
      bool CreateDepthStencilBuffer( );

   private:
      Window*                 m_window;
      ID3D11Device*           m_device;
      ID3D11DeviceContext*    m_immediateContext;

      // Deferred Contexts
      std::array<ID3D11DeviceContext*, 3> m_deferredContexts;

      // Back Buffer Variables
      IDXGISwapChain*           m_swapChain;
      ID3D11RenderTargetView*   m_renderTargetView;
      DepthStencilBufferDX11*   m_depthStencilBuffer;

      // Pre light pass Rendering
      GBuffer*          m_gBuffer;
      GBufferPass*      m_gBufferPass;
      RenderTargetDX11* m_lightBuffer;
      LightBufferPass*  m_lightBufferPass;
      ShadingPass*      m_shadingPass;
      Quad*             m_screenQuad;

      // Test Rendering Pass
      TestRenderPass*   m_testPass;

      // Renderable objects
      std::vector<MeshRenderComponent*> m_meshRenderComponents;
      std::vector<LightComponent*>      m_lightComponents;
      MaterialMap                       m_materialMap;
      std::vector<CameraComponent*>     m_cameras;

      CameraComponent*                  m_mainCamera;

      // Render State
      bool      m_bDepthStencilEnabled;
      Vector4   m_clearColor;

      Viewport*  m_viewport; // @TODO: Multiple Viewports

      // Rasterizer State
      RasterizerState*   m_defaultState;

   };
}