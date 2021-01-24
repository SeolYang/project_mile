#pragma once
#include "Resource/Resource.h"
#include "Rendering/RenderingCore.h"
#include "Core/Logger.h"

namespace Mile
{
   DECLARE_LOG_CATEGORY_EXTERN(MileRenderTexture, Log);

   class RenderTargetDX11;
   class DepthStencilBufferDX11;
   class MEAPI RenderTexture : public Resource
   {
   public:
      RenderTexture(Context* context, const String& path) :
         m_bIsDirty(true),
         m_width(1),
         m_height(1),
         m_colorFormat(EColorFormat::R8G8B8A8_UNORM),
         m_bEnableStencil(true),
         m_renderTarget(nullptr),
         m_depthStencil(nullptr),
         Resource(context, path, ResourceType::RenderTexture)
      {
      }

      ~RenderTexture();

      virtual bool Init() override;
      virtual bool SaveTo(const String& filePath) override;

      void SetWidth(UINT32 width) 
      {
         if (m_width != width)
         { 
            m_bIsDirty = true;
            m_width = width;
         } 
      }

      UINT32 GetWidth() const { return m_width; }

      void SetHeight(UINT32 height)
      {
         if (m_height != height)
         {
            m_bIsDirty = true;
            m_height = height;
         }
      }

      UINT32 GetHeight() const { return m_height; }

      void SetEnableStencil(bool enable)
      {
         if (m_bEnableStencil != enable)
         {
            m_bIsDirty = true;
            m_bEnableStencil = enable;
         }
      }

      bool IsStencilEnabled() const { return m_bEnableStencil; }

      void SetColorFormat(EColorFormat format)
      {
         if (m_colorFormat != format)
         {
            m_bIsDirty = true;
            m_colorFormat = format;
         }
      }

      EColorFormat GetColorFormat() const { return m_colorFormat; }

      RenderTargetDX11* GetRenderTarget();

      virtual json Serialize() const override;
      virtual void DeSerialize(const json& jsonData) override;

   private:
      void GenerateRenderTarget();

   private:
      bool m_bIsDirty;
      UINT32 m_width;
      UINT32 m_height;
      EColorFormat m_colorFormat;
      bool m_bEnableStencil;

      RenderTargetDX11* m_renderTarget;
      DepthStencilBufferDX11* m_depthStencil;

   };
}