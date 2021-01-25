#pragma once
#include "Rendering/RenderingCore.h"
#include "Core/Logger.h"
#include <array>
#include <vector>

namespace Mile
{
   DECLARE_LOG_CATEGORY_EXTERN(MileRendererDX11, Log);

   enum class ERenderContextType : UINT32
   {
      GeometryPass,
      LightingPass,
      PostProcessPass,
      Immediate,
      EnumSize
   };

   enum class EBloomType : UINT32
   {
      Box,
      Gaussian,
      None,
      EnumSize
   };

   constexpr size_t REQUIRED_RENDERCONTEXT_NUM = (size_t)ERenderContextType::EnumSize - 1;
   constexpr unsigned int DYNAMIC_CUBEMAP_SIZE = 512;
   constexpr unsigned int IRRADIANCEMAP_SIZE = 32;
   constexpr unsigned int PREFILTERED_CUBEMAP_SIZE = 512;
   constexpr unsigned int PREFILTERED_CUBEMAP_MAX_MIPLEVELS = 6 + 1;
   constexpr float DEFAULT_EXPOSURE_FACTOR = 1.0f;
   constexpr float DISABLE_TONE_MAPPING = -1.0f;
   constexpr float DEFAULT_GAMMA_FACTOR = 2.2f;
   constexpr float DISABLE_GAMMA_CORRECTION = -1.0f;
   constexpr float DEFAULT_AO_FACTOR = 0.6f;
   constexpr unsigned int DEFAULT_GAUSSIAN_BLOOM_AMOUNT = 4;
   constexpr float DEFAULT_GAUSSIAN_BLOOM_INTENSITY = 0.8f;
   constexpr float DEFAULT_GAUSSIAN_BLOOM_THRESHOLD = 0.8f;
   constexpr float DEFAULT_GAUSSIAN_BLOOM_DEPTH_THRESHOLD = 0.999f;
   constexpr float SSAO_NOISE_TEXTURE_RES = 4.0f;
   constexpr unsigned int SSAO_KERNEL_SIZE = 64;
   constexpr float DEFAULT_SSAO_RADIUS = 2.0f;
   constexpr float DEFAULT_SSAO_BIAS = 0.01f;
   constexpr float DEFAULT_SSAO_MAGNITUDE = 1.1f;

   class OnWindowResizeDelegate;
   class OnWindowMinimizedDelegate;
   class DepthStencilBufferDX11;
   class RenderTargetDX11;
   class Mesh;
   class Quad;
   class Cube;
   class GBuffer;
   class Viewport;
   class Texture2D;
   class Texture2dDX11;
   class Equirect2CubemapPass;
   class IrradianceConvPass;
   class PrefilteringPass;
   class IntegrateBRDFPass;
   class GeometryPass;
   class LightingPass;
   class ConvertGBufferToViewPass;
   class SSAOPass;
   class SSAOBlurPass;
   class AmbientEmissivePass;
   class SkyboxPass;
   class BoxBloomPass;
   class ExtractBrightnessPass;
   class GaussianBlurPass;
   class BlendingPass;
   class ToneMappingPass;
   class Window;
   class World;
   class Entity;
   class MeshRenderComponent;
   class LightComponent;
   class CameraComponent;
   class Material;
   class RasterizerState;
   class BlendState;
   class DepthStencilState;
   class CommandListDX11;
   class DynamicCubemap;
   class RenderTexture;
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

      void SetVsync(bool enable = false);
      bool IsVsyncEnabled() const { return m_vsyncEnabled; }

      /* Rendering Methods **/
      void Render();

      void Clear(ID3D11DeviceContext& deviceContext);
      void ClearDepthStencil(ID3D11DeviceContext& deviceContext);

      ID3D11Device* GetDevice() { return m_device; }
      ID3D11DeviceContext* GetImmediateContext() { return m_immediateContext; }

      void Present();

      void SetBackbufferAsRenderTarget(ID3D11DeviceContext& deviceContext);
      void SetClearColor(Vector4 clearColor);
      Vector4 GetClearColor() const { return m_clearColor; }

      void SetEquirectangularMap(Texture2D* texture);
      Texture2D* GetEquirectangularMap() const { return m_equirectangularMap; }
      void SetComputeIBLAsRealtime(bool bComputeIBLAsRealtime = false);
      bool IsComputeIBLAsRealtime() const { return m_bAlwaysComputeIBL; }

      void SetAmbientOcclusionFactor(float factor) { m_aoFactor = factor; }
      float GetAmbientOcclusionFactor() const { return m_aoFactor; }

      void SetGammaFactor(float gammaFactor) { m_gammaFactor = gammaFactor; }
      float GetGammaFactor() const { return m_gammaFactor; }

      void SetExposure(float exposureFactor) { m_exposureFactor = exposureFactor; }
      float GetExposureFactor() const { return m_exposureFactor; }

      void SetBloomType(EBloomType type) { m_bloomType = type; }
      EBloomType GetBloomType() const { return m_bloomType; }

      void SetGaussianBloomAmount(unsigned int amount) { m_gaussianBloomAmount = amount; }
      unsigned int GetGaussianBloomAmount() const { return m_gaussianBloomAmount; }

      void SetGaussianBloomIntensity(float intensity) { m_gaussianBloomIntensity = intensity; }
      float GetGaussianBloomIntensity() const { return m_gaussianBloomIntensity; }

      void SetGaussianBloomThreshold(float threshold) { m_gaussianBloomThreshold = threshold; }
      float GetGaussianBloomThreshold() const { return m_gaussianBloomThreshold; }

      void SetGaussianBloomDepthThreshold(float threshold) { m_gaussianBloomThreshold = threshold; }
      float GetGaussianBloomDepthThreshold() const { return m_gaussianBloomDepthThreshold; }

      void SetSSAOEanble(bool bEnable) { m_bEnableSSAO = bEnable; }
      bool IsSSAOEnabled() const { return m_bEnableSSAO; }

      void SetSSAORadius(float radius) { m_ssaoRadius = radius; }
      float GetSSAORadius() const { return m_ssaoRadius; }

      void SetSSAOBias(float bias) { m_ssaoBias = bias; }
      float GetSSAOBias() const { return m_ssaoBias; }

      void SetSSAOMagnitude(float mag) { m_ssaoMagnitude = mag; }
      float GetSSAOMagnitude() const { return m_ssaoMagnitude; }

      Mesh* GetCubeMesh() const { return reinterpret_cast<Mesh*>(m_cubeMesh); }
      Mesh* GetQuadMesh() const { return reinterpret_cast<Mesh*>(m_screenQuad); }

      bool IsRenderedFrame() const { return m_bIsRendered; }

      Vector2 GetReferenceResolution() const { return m_referenceResolution; }
      float GetReferenceAspectRatio() const { return (m_referenceResolution.x / m_referenceResolution.y); }

      void OnWindowResize(unsigned int width, unsigned int height);
      void OnWindowMinimized();

   private:
      /* Initialization methods **/
      bool CreateDeviceAndSwapChain();
      bool CreateDepthStencilBuffer();

      bool InitAPI();
      bool InitPBR();
      bool InitSSAO();
      bool InitPostProcess();
      bool InitStates();

      /* Rendering Workflow **/
      ID3D11CommandList* RunGeometryPass(ID3D11DeviceContext* deviceContextPtr);
      ID3D11CommandList* RunLightingPass(ID3D11DeviceContext* deviceContextPtr);
      ID3D11CommandList* RunPostProcessPass(ID3D11DeviceContext* deviceContextPtr);

      /* Pre Computation **/
      void CalculateDiffuseIrradiance(ID3D11DeviceContext& deviceContext);
      void ConvertEquirectToCubemap(ID3D11DeviceContext& deviceContext, const std::array<Matrix, CUBE_FACES>& captureMatrix);
      void SolveDiffuseIntegral(ID3D11DeviceContext& deviceContext, const std::array<Matrix, CUBE_FACES>& captureMatrix);
      void ComputePrefilteredEnvMap(ID3D11DeviceContext& deviceContext, const std::array<Matrix, CUBE_FACES>& captureMatrix);
      void IntegrateBRDF(ID3D11DeviceContext& deviceContext);

      /* Lighting **/
      void RenderLight(ID3D11DeviceContext& deviceContext);

      /* Post-Process **/
      GBuffer* ConvertGBufferToViewSpace(ID3D11DeviceContext& deviceContext, GBuffer* gBuffer);
      RenderTargetDX11* RenderAmbientEmissive(ID3D11DeviceContext& deviceContext, RenderTargetDX11* renderTarget);
      RenderTargetDX11* RenderSkybox(ID3D11DeviceContext& deviceContext, RenderTargetDX11* renderTarget);
      RenderTargetDX11* SSAO(ID3D11DeviceContext& deviceContext, GBuffer* gBuffer, float radius, float bias);
      RenderTargetDX11* SSAOBlur(ID3D11DeviceContext& deviceContext, RenderTargetDX11* ssaoInput);
      RenderTargetDX11* ExtractBrightness(ID3D11DeviceContext& deviceContext, GBuffer* gBuffer, RenderTargetDX11* renderBuffer, float depthThreshold, float threshold);
      RenderTargetDX11* GaussianBlur(ID3D11DeviceContext& deviceContext, RenderTargetDX11* renderBuffer, unsigned int gaussianAmount);
      RenderTargetDX11* Bloom(ID3D11DeviceContext& deviceContext, RenderTargetDX11* renderBuffer);
      RenderTargetDX11* BoxBloom(ID3D11DeviceContext& deviceContext, RenderTargetDX11* renderBuffer);
      RenderTargetDX11* GaussianBloom(ID3D11DeviceContext& deviceContext, RenderTargetDX11* renderBuffer);
      RenderTargetDX11* Blending(ID3D11DeviceContext& deviceContext, RenderTargetDX11* srcBuffer, RenderTargetDX11* destBuffer, float srcRatio = 1.0f, float destRatio = 1.0f);
      void ToneMappingWithGammaCorrection(ID3D11DeviceContext& deviceContext, RenderTargetDX11* renderBuffer);
   
      /* Helper Methods **/
      /* @TODO Entity 배열을 받는게 아닌 World Subsystem만 가져와서 특정 컴포넌트만 찾을 수 있도록 하기. **/
      /*
       * @brief   주어진 Entity의 배열에서 Mesh Renderer 컴포넌트와 Material들을 취득하여 내부적으로 저장합니다.
       * @warn    이번 프레임에서만 사용되어야하며 다음 프레임에서는 새로 취득해야할수도 있습니다.
       **/
      void AcquireMeshRenderersAndMaterial(World* world);

      /*
       * @brief   주어진 Entity의 배열에서 Light 컴포넌트를 취득하여 내부적으로 저장합니다.
       * @warn    이번 프레임에서만 사용되어야하며 다음 프레임에서는 새로 취득해야할수도 있습니다.
       **/
      void AcquireLights(World* world);

      /*
      * @brief    주어진 Entity의 배열에서 Camera 컴포넌트를 취득하여 내부적으로 저장합니다.
      * @warn    이번 프레임에서만 사용되어야하며 다음 프레임에서는 새로 취득해야할수도 있습니다.
      **/
      void AcquireCameras(World* world);

      ID3D11DeviceContext* GetRenderContextByType(ERenderContextType type);

      void SetDepthStencilEnable(ID3D11DeviceContext& deviceContext, bool bDepthEnabled);
      bool IsDepthStencilEnabled() const { return m_bDepthStencilEnabled; }

   private:
      Window* m_window;
      OnWindowResizeDelegate* m_onWindowResize;
      OnWindowMinimizedDelegate* m_onWindowMinimized;

      Vector2 m_referenceResolution;

      ID3D11Device* m_device;
      ID3D11DeviceContext* m_immediateContext;

      /* Rendering settings **/
      bool m_vsyncEnabled;

      /* Deferred Contexts **/
      std::array<ID3D11DeviceContext*, REQUIRED_RENDERCONTEXT_NUM> m_deferredContexts;

      /* Back Buffer Variables **/
      IDXGISwapChain* m_swapChain;
      ID3D11RenderTargetView* m_backBufferRTV;
      DepthStencilBufferDX11* m_backBufferDepthStencilBuffer;
      RenderTargetDX11* m_backBuffer;

      /* Reference Primitive Meshes **/
      Quad* m_screenQuad;
      Cube* m_cubeMesh;

      /* PBS Workflow **/
      DepthStencilBufferDX11* m_dummyDepthStencilBuffer;
      GBuffer* m_gBuffer;
      GeometryPass* m_geometryPass;
      LightingPass* m_lightingPass;
      RenderTargetDX11* m_lightingPassRenderBuffer;

      /** Cubemap / Environment Map */
      bool  m_bCubemapDirtyFlag; // Diffuse Irradiance 와 Specular IBL 의 계산이 끝난 후에 false로 설정하여야 한다.
      Equirect2CubemapPass* m_equirectToCubemapPass;
      Texture2D* m_equirectangularMap;
      DynamicCubemap* m_envMap;

      /** Diffuse Irradiance  */
      bool  m_bAlwaysComputeIBL;
      IrradianceConvPass* m_irradianceConvPass;
      DynamicCubemap* m_irradianceMap;

      /** Specular IBL */
      PrefilteringPass* m_prefilteringPass;
      DynamicCubemap* m_prefilterdEnvMap;

      IntegrateBRDFPass* m_integrateBRDFPass;
      RenderTargetDX11* m_brdfLUT;

      ConvertGBufferToViewPass* m_convertGBufferToViewPass;
      GBuffer* m_viewSpaceGBuffer;

      /* SSAO **/
      bool m_bEnableSSAO;
      std::array<Vector3, SSAO_KERNEL_SIZE> m_ssaoKernel;
      Texture2dDX11* m_ssaoNoise;
      Vector2 m_noiseScale;
      float m_ssaoRadius;
      float m_ssaoBias;
      float m_ssaoMagnitude;
      SSAOPass* m_ssaoPass;
      SSAOBlurPass* m_ssaoBlurPass;

      /** Ambient Emissive Pass */
      float m_aoFactor;
      AmbientEmissivePass* m_ambientEmissivePass;
      RenderTargetDX11* m_ambientEmissivePassRenderBuffer;

      SkyboxPass* m_skyboxPass;

      /** Post-Process */
      RenderTargetDX11* m_hdrBuffer;

      /** Bloom */
      EBloomType    m_bloomType;
      BoxBloomPass* m_boxBloomPass;

      float        m_gaussianBloomIntensity;
      unsigned int m_gaussianBloomAmount;
      float        m_gaussianBloomThreshold;
      float        m_gaussianBloomDepthThreshold;
      ExtractBrightnessPass* m_extractBrightnessPass;
      GaussianBlurPass*      m_gaussianBlurPass;

      /** Blending */
      BlendingPass* m_blendingPass;

      /** Tone Mapping */
      ToneMappingPass* m_toneMappingPass;
      float m_exposureFactor;
      float m_gammaFactor;

      /* Renderable objects **/
      std::vector<MeshRenderComponent*> m_meshRenderComponents;
      std::vector<LightComponent*>      m_lightComponents;
      std::vector<CameraComponent*>     m_cameras;
      MaterialMap                       m_materialMap;
      CameraComponent* m_mainCamera;

      /* Render State **/
      bool      m_bDepthStencilEnabled;
      Vector4   m_clearColor;
      BlendState* m_additiveBlendState;
      BlendState* m_defaultBlendState;
      DepthStencilState* m_depthLessEqual;
      DepthStencilState* m_depthDisable;

      // @TODO: Multiple Viewports
      Viewport* m_viewport;

      /* Rasterizer State **/
      RasterizerState* m_defaultRasterizerState;
      RasterizerState* m_noCulling;

      /** Output */
      RenderTargetDX11* m_outputRenderTarget;

      bool m_bIsRendered;

   };
}