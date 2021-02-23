#include "Resource/Texture2D.h"
#include "Resource/TextureLoader.h"
#include "Rendering/Texture2dDX11.h"
#include "Rendering/RendererDX11.h"
#include "Core/Engine.h"
#include "Core/Context.h"

namespace Mile
{
   DEFINE_LOG_CATEGORY(MileTexture2D);

   Texture2D::Texture2D(ResourceManager* resMng) :
      m_rawData(nullptr),
      m_rawTexture(nullptr),
      m_width(0),
      m_height(0),
      m_channels(0),
      m_bitDepth(0),
      m_bitPerChannel(0),
      m_bIsHDR(false),
      Resource(resMng, ResourceType::Texture2D)
   {
   }

   Texture2D::~Texture2D()
   {
      SafeDelete(m_rawTexture);
   }

   bool Texture2D::Init(const String& filePath)
   {
      if (Resource::Init(filePath))
      {
         auto info = TextureLoader::LoadTexture(m_path, m_ext);
         m_rawData = std::get<TextureInfoTag::RAWDATA>(info);
         m_width = std::get<TextureInfoTag::WIDTH>(info);
         m_height = std::get<TextureInfoTag::HEIGHT>(info);
         m_channels = std::get<TextureInfoTag::CHANNELS>(info);
         m_bitPerChannel = std::get<TextureInfoTag::BIT_PER_CHANNEL>(info);
         m_bIsHDR = std::get<TextureInfoTag::IS_HDR_TEXTURE>(info);

         if (m_rawData == nullptr)
         {
            ME_LOG(MileTexture2D, Warning, TEXT("Failed to load Texture2D from ") + m_path);
            return false;
         }

         if (!InitRawTexture())
         {
            return false;
         }

         SucceedInit();
         return true;
      }

      ME_LOG(MileTexture2D, Warning, TEXT("Is already initialized."));
      return false;
   }

   bool Texture2D::Save(const String& filePath)
   {
      return false;
   }

   bool Texture2D::InitRawTexture()
   {
      if (m_rawData == nullptr || m_rawTexture != nullptr)
      {
         return false;
      }

      auto renderer = Engine::GetRenderer();
      m_rawTexture = new Texture2dDX11(renderer);
      if (!m_rawTexture->Init(m_width, m_height, m_channels, m_rawData, m_bIsHDR ? DXGI_FORMAT_R32G32B32A32_FLOAT : DXGI_FORMAT_B8G8R8A8_UNORM))
      {
         ME_LOG(MileTexture2D, Warning, TEXT("Failed to initialize Raw Texture: ") + m_path);
         SafeDelete(m_rawTexture);
         return false;
      }

      return true;
   }
}