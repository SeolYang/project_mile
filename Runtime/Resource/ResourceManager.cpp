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
      Context* context = GetContext();
      if (SubSystem::Init())
      {
         m_cache = std::make_unique<ResourceCache>(context);
         if (m_cache == nullptr)
         {
            return false;
         }

         MELog(context, TEXT("ResourceManager"), ELogType::MESSAGE, TEXT("Resource Manager Initialized!"), true);
         SubSystem::InitSucceed();
         return true;
      }

      MELog(context, TEXT("ResourceManager"), ELogType::FATAL, TEXT("Failed to initialize Resource Manager."), true);
      return false;
   }

   void ResourceManager::DeInit()
   {
      if (IsInitialized())
      {
         ClearCache();
         MELog(GetContext(), TEXT("ResourceManager"), ELogType::MESSAGE, TEXT("Resource Manager deinitialized!"), true);
         SubSystem::DeInit();
      }
   }

   void ResourceManager::ClearCache()
   {
      m_cache->Clear();
   }

}