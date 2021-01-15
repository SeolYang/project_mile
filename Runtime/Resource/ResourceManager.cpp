#include "Resource/ResourceManager.h"
#include "Resource/ResourceCache.h"
#include "Core/Context.h"

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
      Context* context = GetContext();
      if (SubSystem::Init())
      {
         m_cache = std::make_unique<ResourceCache>(context);
         if (m_cache == nullptr)
         {
            return false;
         }

         MELog(context, 
            MILE_RESOURCE_MANAGER_LOG_CATEGORY,
            ELogType::MESSAGE,
            TEXT("Resource Manager Initialized!"));
         SubSystem::InitSucceed();
         return true;
      }

      MELog(context, 
         MILE_RESOURCE_MANAGER_LOG_CATEGORY,
         ELogType::FATAL, 
         TEXT("Failed to initialize Resource Manager."));
      return false;
   }

   void ResourceManager::DeInit()
   {
      if (IsInitialized())
      {
         ClearCache();
         MELog(GetContext(),
            MILE_RESOURCE_MANAGER_LOG_CATEGORY,
            ELogType::MESSAGE, 
            TEXT("Resource Manager deinitialized!"));
         SubSystem::DeInit();
      }
   }

   void ResourceManager::ClearCache()
   {
      m_cache->Clear();
      MELog(GetContext(),
         MILE_RESOURCE_MANAGER_LOG_CATEGORY,
         ELogType::MESSAGE,
         TEXT("Resource cache has been cleared."));
   }

}