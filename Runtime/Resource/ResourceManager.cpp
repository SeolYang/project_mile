#include "Resource/ResourceManager.h"
#include "Resource/ResourceCache.h"
#include "Core/Context.h"
#include "Core/Logger.h"

namespace Mile
{
   ResourceManager::ResourceManager(Context* context) :
      SubSystem(context)
   {
   }

   ResourceManager::~ResourceManager()
   {
      DeInit();
   }

   bool ResourceManager::Init()
   {
      if (m_context == nullptr || m_bIsInitialized)
      {
         return false;
      }

      m_cache = std::make_unique<ResourceCache>(m_context);
      if (m_cache == nullptr)
      {
         return false;
      }

      MELog(m_context, TEXT("ResourceManager"), ELogType::MESSAGE, TEXT("Resource Manager Initialized!"), true);
      m_bIsInitialized = true;
      return true;
   }

   void ResourceManager::DeInit()
   {
      if (m_bIsInitialized)
      {
         ClearCache();
         SubSystem::DeInit();
         MELog(m_context, TEXT("ResourceManager"), ELogType::MESSAGE, TEXT("Resource Manager deinitialized!"), true);
      }
   }

   void ResourceManager::ClearCache()
   {
      m_cache->Clear();
   }

}