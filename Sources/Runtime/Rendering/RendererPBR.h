#pragma once
#include "Rendering/RendererDX11.h"
#include "Rendering/FrameResources.h"
#include "Elaina/FrameGraph.h"

namespace Mile
{
   DECLARE_LOG_CATEGORY_EXTERN(MileRendererPBR, Log);

   namespace RendererPBRConstants
   {
      constexpr unsigned int ConvertedEnvMapSize = 512;
      constexpr unsigned int IrradianceMapSize = 32;
      constexpr unsigned int PrefilteredEnvMapSize = 512;
      constexpr unsigned int PrefilteredEnvMapMaxMipLevels = 6 + 1;
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

   struct MEAPI SSAOParams
   {
      Vector4 Samples[RendererPBRConstants::SSAOKernelSize];
      Vector2 NoiseScale;
      std::vector<Vector4> Noise = std::vector<Vector4>(RendererPBRConstants::SSAONoiseTextureSize * RendererPBRConstants::SSAONoiseTextureSize);
      float Radius = 2.0f;
      float Bias = 0.01f;
      float Magnitude = 1.1f;
   };

   class MEAPI RendererPBR : public RendererDX11
   {
   public:
      RendererPBR(Context* context, size_t maximumThreads);
      virtual ~RendererPBR();

      bool Init(Window& window) override;
      bool SetupSSAOParams();

      SSAOParams& GetSSAOParams() { return m_ssaoParams; }
      SSAOParams GetSSAOParams() const { return m_ssaoParams; }
      void SetSSAOParams(const SSAOParams& ssaoParams)
      {
         m_ssaoParams = ssaoParams;
      }

   protected:
      void RenderImpl(const World& world) override;

   private:
      bool InitShader();
      bool InitFrameGraph();

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
      SSAOParams m_ssaoParams;

   };
}