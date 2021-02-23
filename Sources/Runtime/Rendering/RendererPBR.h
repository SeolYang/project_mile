#pragma once
#include "Rendering/RendererDX11.h"
#include "Rendering/FrameResources.h"
#include "Elaina/FrameGraph.h"

namespace Mile
{
   DECLARE_LOG_CATEGORY_EXTERN(MileRendererPBR, Log);

   namespace RendererPBRConstants
   {
      constexpr unsigned int ConvertedEnvMapSize = 1024;
      constexpr unsigned int IrradianceMapSize = 32;
      constexpr unsigned int PrefilteredEnvMapSize = 256;
      constexpr unsigned int BRDFLUTSize = 512;
      constexpr unsigned int SSAOKernelSize = 64;
      constexpr unsigned int SSAONoiseTextureSize = 4;
   }

   struct MEAPI RenderPassDataBase
   {
      RendererDX11* Renderer = nullptr;
      VertexShaderResource* VertexShader = nullptr;
      PixelShaderResource* PixelShader = nullptr;
   };

   DEFINE_CONSTANT_BUFFER(SSAOBaseData)
   {
      Vector4 Samples[RendererPBRConstants::SSAOKernelSize];
      Vector2 NoiseScale = Vector2(1.0f, 1.0f);
   };

   struct MEAPI SSAOParams
   {
      std::vector<Vector4> Noise = std::vector<Vector4>(RendererPBRConstants::SSAONoiseTextureSize * RendererPBRConstants::SSAONoiseTextureSize);
      float Radius = 2.0f;
      float Bias = 0.01f;
      float Magnitude = 1.1f;
   };

   struct MEAPI BloomParams
   {
      float BrightnessThreshold = 0.0f;
      unsigned int BlurAmount = 4;
      float Intensity = 0.3f;
   };

   struct MEAPI ToneMappingParams
   {
      float ExposureFactor = 2.4f;
      float GammaFactor = 2.2f;
   };

   enum class MEAPI ESkyboxType
   {
      EnvironmentMap,
      IrradianceMap,
   };

   class MEAPI RendererPBR : public RendererDX11
   {
   public:
      RendererPBR(Context* context, size_t maximumThreads);
      virtual ~RendererPBR();

      bool Init(Window& window) override;

      SSAOParams& GetSSAOParams() { return m_ssaoParams; }
      SSAOParams GetSSAOParams() const { return m_ssaoParams; }

      BloomParams& GetBloomParams() { return m_bloomParams; }
      BloomParams GetBloomParams() const { return m_bloomParams; }

      ToneMappingParams& GetToneMappingParams() { return m_toneMappingParams; }
      ToneMappingParams GetToneMappingParams() const { return m_toneMappingParams; }

      float& GetAmbientIntensty() { return m_ambientIntensity; }
      float GetAmbientIntensty() const { return m_ambientIntensity; }

      bool& SSAOEnabled() { return m_bSSAOEnabled; }
      bool IsSSAOEnabled() const { return m_bSSAOEnabled; }

      GBuffer* GetGBuffer() const { return m_gBuffer; }
      RenderTargetDX11* GetSSAOBuffer() const { return m_blurredSSAO; }
      RenderTargetDX11* GetExtractedBrightnessBuffer() const { return m_extractedBrightness; }
      RenderTargetDX11* GetDebugDepthBuffer() const { return m_depthDebugBuffer; }
      RenderTargetDX11* GetDebugSSAOBuffer() const { return m_ssaoDebugBuffer; }
      RenderTargetDX11* GetDebugLightingBuffer() const { return m_lightingDebugBuffer; }

   protected:
      void RenderImpl(const World& world) override;
      void OnRenderResolutionChanged() override;

      void AcquireRenderResources(const World& world);

      static void RenderMeshes(
         RendererDX11* renderer,
         bool bClearGBuffer,
         const Meshes& meshes,
         size_t offset,
         size_t num,
         VertexShaderDX11* vertexShader, PixelShaderDX11* pixelShader, SamplerDX11* sampler, 
         GBuffer* gBuffer, ConstantBufferDX11* transformBuffer, ConstantBufferDX11* materialParamsBuffer, 
         RasterizerState* rasterizeState, Viewport* viewport,
         CameraRef camera,
         size_t threadIdx = 0);

   private:
      bool InitShader();
      bool InitFrameGraph();

      void SetupRenderResources();
      void SetupSSAOParams();

   private:
      Elaina::FrameGraph m_frameGraph;

      /** Basic Render Meshes */
      MeshRef m_cubeMesh;
      MeshRef m_quadMesh;

      /** Shaders */
      VertexShaderDX11* m_geometryPassVS;
      PixelShaderDX11* m_geometryPassPS;

      VertexShaderDX11* m_convertSkyboxPassVS;
      PixelShaderDX11* m_convertSkyboxPassPS;

      VertexShaderDX11* m_diffuseIntegralPassVS;
      PixelShaderDX11* m_diffuseIntegralPassPS;

      VertexShaderDX11* m_prefilterEnvPassVS;
      PixelShaderDX11* m_prefilterEnvPassPS;

      VertexShaderDX11* m_integrateBRDFPassVS;
      PixelShaderDX11* m_integrateBRDFPassPS;

      VertexShaderDX11* m_lightingPassVS;
      PixelShaderDX11* m_lightingPassPS;

      VertexShaderDX11* m_gBufferToViewSpacePassVS;
      PixelShaderDX11* m_gBufferToViewSpacePassPS;

      VertexShaderDX11* m_ssaoPassVS;
      PixelShaderDX11* m_ssaoPassPS;

      VertexShaderDX11* m_ssaoBlurPassVS;
      PixelShaderDX11* m_ssaoBlurPassPS;

      VertexShaderDX11* m_ambientEmissivePassVS;
      PixelShaderDX11* m_ambientEmissivePassPS;

      VertexShaderDX11* m_skyboxPassVS;
      PixelShaderDX11* m_skyboxPassPS;

      VertexShaderDX11* m_extractBrightnessPassVS;
      PixelShaderDX11* m_extractBrightnessPassPS;

      VertexShaderDX11* m_gaussBloomPassVS;
      PixelShaderDX11* m_gaussBloomPassPS;

      VertexShaderDX11* m_printTextureVS;
      PixelShaderDX11* m_printTexturePS;

      VertexShaderDX11* m_toneMappingVS;
      PixelShaderDX11* m_toneMappingPS;

      VertexShaderDX11* m_debugDepthSSAOVS;
      PixelShaderDX11* m_debugDepthSSAOPS;

      GBuffer* m_gBuffer;
      RenderTargetDX11* m_hdrBuffer;

      /** External Resources; Don't delete in renderer! */
      /** Per Frame Datas */
      std::vector<CameraComponent*> m_cameras;
      CameraComponent* m_targetCamera;
      Lights m_lights;
      Meshes m_meshes;
      MaterialMap m_materialMap;
      RenderTargetDX11* m_outputRenderTarget;

      /** Skybox/IBL */
      Texture2D* m_skyboxTexture;
      Texture2D* m_oldSkyboxTexture;
      bool m_bPrecomputeIBL;

      DynamicCubemapRef m_environmentMap;
      DynamicCubemapRef m_irradianceMap;
      DynamicCubemapRef m_prefilteredEnvMap;
      RenderTargetRef   m_brdfLUT;

      /** Post-process */
      bool m_bSSAOEnabled;
      SSAOBaseData m_ssaoBaseData;
      ConstantBufferRef m_ssaoBaseDataBuffer;
      SSAOParams m_ssaoParams;
      float m_ambientIntensity;
      RenderTargetDX11* m_ssao;
      RenderTargetDX11* m_blurredSSAO;
      Texture2dDX11* m_ssaoNoiseTex;

      ESkyboxType m_renderSkyboxType;

      BloomParams m_bloomParams;
      ToneMappingParams m_toneMappingParams;
      RenderTargetDX11* m_extractedBrightness;
      std::array<RenderTargetDX11*, 2> m_pingPongBuffers;

      /** Debug */
      RenderTargetDX11* m_depthDebugBuffer;
      RenderTargetDX11* m_ssaoDebugBuffer;
      RenderTargetDX11* m_lightingDebugBuffer;

   };
}