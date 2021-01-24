#include "Resource/ResourceManager.h"
#include "Resource/ResourceCache.h"
#include "Core/Context.h"

namespace Mile
{
   DEFINE_LOG_CATEGORY(MileResourceManager);

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

         ME_LOG(MileResourceManager, ELogVerbosity::Log, TEXT("Resource Manager Initialized!"));
         SubSystem::InitSucceed();
         return true;
      } 

      ME_LOG(MileResourceManager, ELogVerbosity::Fatal,
         TEXT("Failed to initialize Resource Manager!"));
      return false;
   }

   void ResourceManager::DeInit()
   {
      if (IsInitialized())
      {
         ClearCache();
         ME_LOG(MileResourceManager, ELogVerbosity::Log, TEXT("Resource Manager deinitialized."));
         SubSystem::DeInit();
      }
   }

   bool ResourceManager::IsValid(Resource* target) const
   {
      return m_cache->IsValid(target);
   }

   void ResourceManager::ClearCache()
   {
      m_cache->Clear();
      ME_LOG(MileResourceManager, ELogVerbosity::Log, TEXT("Resource cache has been cleared."));
   }
}