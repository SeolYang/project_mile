#pragma once
#include "Resource/Resource.h"
#include "Resource/ResourceCache.h"
#include "Core/Logger.h"

namespace Mile
{
   DECLARE_LOG_CATEGORY_STATIC(MileResourceManager, ELogVerbosity::Log);

   class MEAPI ResourceManager : public SubSystem
   {
   public:
      ResourceManager(Context* context);
      virtual ~ResourceManager();

      virtual bool Init() override;
      virtual void DeInit() override;

      template < typename Ty >
      Ty* Load(const String& relativePath, bool bDoNotLeaveCachingLog = false)
      {
         if (!relativePath.empty())
         {
            Context* context = GetContext();
            if (m_cache->HasByPath(relativePath))
            {
               if (!bDoNotLeaveCachingLog)
               {
                  ME_LOG(MileResourceManager, ELogVerbosity::Log, TEXT("Successfully load resource from Cache : ") + relativePath);
               }

               return GetByPath<Ty>(relativePath);
            }

            auto newResource = new Ty(context, relativePath);
            if (newResource->Init())
            {
               m_cache->Add(static_cast<ResourcePtr>(newResource));
               ME_LOG(MileResourceManager, ELogVerbosity::Log, TEXT("Successfully load resource from : ") + relativePath);
               return newResource;
            }

            ME_LOG(MileResourceManager, ELogVerbosity::Warning, TEXT("Failed to load resource from : ") + relativePath);
            SafeDelete(newResource);
         }

         return nullptr;
      }

      template < typename Ty >
      Ty* GetByPath(const String& filePath)
      {
         if (m_cache->HasByPath(filePath))
         {
            return static_cast<Ty*>(m_cache->GetByPath(filePath));
         }

         return nullptr;
      }

      template < typename Ty >
      Ty* GetByName(const String& name)
      {
         if (m_cache->HasByName(name))
         {
            return static_cast<Ty*>(m_cache->GetByName(name));
         }

         return nullptr;
      }

      template < typename Ty >
      Ty* Create(const String& relativePath, bool bSaveResource = true)
      {
         if (!relativePath.empty())
         {
            Context* context = GetContext();
            auto res = Load<Ty>(relativePath);
            if (res != nullptr)
            {
               ME_LOG(MileResourceManager, Log, relativePath + TEXT(" is already exist."));
               return res;
            }

            auto newResource = new Ty(context, relativePath);
            if (bSaveResource)
            {
               if (newResource->Save())
               {
                  ME_LOG(MileResourceManager, Log, relativePath + TEXT(" is successfully created."));
                  m_cache->Add(static_cast<Resource*>(newResource));
                  return newResource;
               }
            }
            else
            {
               m_cache->Add(static_cast<Resource*>(newResource));
               return newResource;
            }


            ME_LOG(MileResourceManager, Fatal, TEXT("Failed to create resource! : ") + relativePath);
            SafeDelete(newResource);
         }

         return nullptr;
      }

      bool IsValid(Resource* target) const;

      void ClearCache();

   private:
      ResourceCachePtr    m_cache;

   };
}