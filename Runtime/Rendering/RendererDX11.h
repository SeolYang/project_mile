#pragma once
#include "Rendering/RenderingCore.h"
#include "Rendering/Viewport.h"
#include <array>

namespace Mile
{
   enum class ERenderContextType : UINT32
   {
      GeometryPass,
      LightingPass,
      PostProcessPass,
      Immediate,
      EnumSize
   };

   constexpr size_t RequireDeferredContextNum = (size_t)ERenderContextType::EnumSize - 1;

   class DepthStencilBufferDX11;
   class RenderTargetDX11;
   class Quad;
   class GBuffer;
   class GeometryPass;
   class LightingPass;
   class Window;
   class World;
   class Entity;
   class MeshRenderComponent;
   class LightComponent;
   class CameraComponent;
   class Material;
   class RasterizerState;
   class CommandListDX11;
   /**
    * @brief	Mile 엔진의 렌더러 서브 시스템입니다. 렌더링시 World 에 생성되어있는 Entity로 부터 Mesh Renderer 정보, 빛 정보,
   *			그리고 카메라 정보와 같은 렌더링에 필요한 정보를 매 프레임마다 취득하여 화면을 렌더링 하는 역할을 가지고 있습니다.
   *			또한 초기화시에 Direct3D 11의 초기화도 같이 진행하게 됩니다.
    */
   class MEAPI RendererDX11 : public SubSystem
   {
      using MaterialMap = std::map<Material*, std::vector<MeshRenderComponent*>>;
   public:
      RendererDX11(Context* context);
      virtual ~RendererDX11();

      virtual bool Init() override;
      virtual void DeInit() override;

      /* Acquire renderable resources **/
      void AcquireMeshRenderersAndMaterial(const std::vector<Entity*>& entities);
      void AcquireLights(const std::vector<Entity*>& entities);
      void AcquireCameras(const std::vector<Entity*>& entities);

      /* Rendering Methods **/
      void Render();

      ID3D11CommandList* RenderGBuffer(ID3D11DeviceContext* deviceContext);
      ID3D11CommandList* RenderLightBuffer(ID3D11DeviceContext* deviceContext);
      ID3D11CommandList* RenderShading(ID3D11DeviceContext* deviceContext);

      ID3D11CommandList* RenderCheckerBoardInterpolate(ID3D11DeviceContext* deviceContext);

      void Clear(ID3D11DeviceContext& deviceContext);
      void ClearDepthStencil(ID3D11DeviceContext& deviceContext);
      void Present();

      ID3D11Device* GetDevice() { return m_device; }
      ID3D11DeviceContext* GetImmediateContext() { return m_immediateContext; }

      void SetClearColor(Vector4 clearColor);
      Vector4 GetClearColor() const { return m_clearColor; }

      void SetDepthStencilEnable(ID3D11DeviceContext& deviceContext, bool bDepthEnabled);
      bool IsDepthStencilEnabled() const { return m_bDepthStencilEnabled; }

      void SetBackbufferAsRenderTarget(ID3D11DeviceContext& deviceContext);

      ID3D11DeviceContext* GetRenderContextByType(ERenderContextType type);

      void SetCheckerBoardRenderingEnable(bool bCheckerBoardRenderingEnabled) { m_bCheckerBoardRenderingEnabled = bCheckerBoardRenderingEnabled; }

   private:
      bool CreateDeviceAndSwapChain();
      bool CreateDepthStencilBuffer();

   private:
      Window* m_window;
      ID3D11Device* m_device;
      ID3D11DeviceContext* m_immediateContext;

      /* Deferred Contexts **/
      std::array<ID3D11DeviceContext*, RequireDeferredContextNum> m_deferredContexts;

      /* Back Buffer Variables **/
      IDXGISwapChain*         m_swapChain;
      ID3D11RenderTargetView* m_renderTargetView;
      DepthStencilBufferDX11* m_depthStencilBuffer;
      RenderTargetDX11*       m_backBuffer;

      /* Pre light pass Rendering **/
      GBuffer*          m_gBuffer;
      GBufferPass*      m_gBufferPass;
      RenderTargetDX11* m_lightBuffer;
      LightBufferPass*  m_lightBufferPass;
      ShadingPass*      m_shadingPass;
      Quad*             m_screenQuad;

      /* CheckerBoard Rendering **/
      bool              m_bCheckerBoardRenderingEnabled;
      RenderTargetDX11* m_checkerBoard;
      CheckerBoardInterpolatePass* m_checkerBoardInterpolatePass;

      /* Renderable objects **/
      std::vector<MeshRenderComponent*> m_meshRenderComponents;
      std::vector<LightComponent*>      m_lightComponents;
      MaterialMap                       m_materialMap;
      std::vector<CameraComponent*>     m_cameras;
      CameraComponent*                  m_mainCamera;

      /* Render State **/
      bool      m_bDepthStencilEnabled;
      Vector4   m_clearColor;

      // @TODO: Multiple Viewports
      Viewport* m_viewport;

      /* Rasterizer State **/
      RasterizerState* m_defaultRasterizerState;

   };
}