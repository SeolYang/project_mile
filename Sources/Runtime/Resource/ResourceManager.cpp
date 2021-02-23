#include "Resource/ResourceManager.h"
#include "Resource/ResourceCache.h"
#include "Resource/ModelLoader.h"
#include "Core/Context.h"

namespace Mile
{
   ResourceManager::ResourceManager(Context* context) :
      m_modelLoader(nullptr),
      SubSystem(context)
   {
   }

   ResourceManager::~ResourceManager()
   {
      DeInit();
   }

   bool ResourceManager::Init()
   {
      if (SubSystem::Init())
      {
         Context* context = GetContext();
         m_cache = std::make_unique<ResourceCache>(context);
         if (m_cache == nullptr)
         {
            return false;
         }

         m_modelLoader = new ModelLoader(this);

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
         SafeDelete(m_modelLoader);
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

   ModelLoader& ResourceManager::GetModelLoader() const
   {
      return (*m_modelLoader);
   }
}