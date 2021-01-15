#pragma once
#include "Resource/Resource.h"
#include "Resource/ResourceCache.h"
#include "Core/Logger.h"

#define MILE_RESOURCE_MANAGER_LOG_CATEGORY TEXT("ResourceManager")

namespace Mile
{
   class MEAPI ResourceManager : public SubSystem
   {
   public:
      ResourceManager(Context* context);
      ~ResourceManager();

      virtual bool Init() override;
      virtual void DeInit() override;

      template < typename Ty >
      Ty* Load(const String& relativePath)
      {
         if (!relativePath.empty())
         {
            Context* context = GetContext();
            if (m_cache->HasByPath(relativePath))
            {
               MELog(context,
                  MILE_RESOURCE_MANAGER_LOG_CATEGORY,
                  ELogType::MESSAGE,
                  TEXT("Successfully load resource from Cache : ") + relativePath);
               return GetByPath<Ty>(relativePath);
            }

            auto newResource = new Ty(context, relativePath);
            if (newResource->Init())
            {
               m_cache->Add(static_cast<ResourcePtr>(newResource));
               MELog(context,
                  MILE_RESOURCE_MANAGER_LOG_CATEGORY,
                  ELogType::MESSAGE,
                  TEXT("Successfully load resource from : ") + relativePath);
               return newResource;
            }

            MELog(context,
               MILE_RESOURCE_MANAGER_LOG_CATEGORY,
               ELogType::WARNING,
               TEXT("Failed to load resource from : ") + relativePath);
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
      Ty* Create(const String& relativePath)
      {
         if (!relativePath.empty())
         {
            Context* context = GetContext();
            auto res = Load<Ty>(relativePath);
            if (res != nullptr)
            {
               MELog(context,
                  MILE_RESOURCE_MANAGER_LOG_CATEGORY,
                  ELogType::WARNING,
                  relativePath + TEXT(" is already exist."));
               return res;
            }

            auto newResource = new Ty(context, relativePath);
            if (newResource->Save())
            {
               MELog(context,
                  MILE_RESOURCE_MANAGER_LOG_CATEGORY,
                  ELogType::WARNING,
                  relativePath + TEXT(" is successfully created."));
               m_cache->Add(static_cast<Resource*>(newResource));
               return newResource;
            }

            MELog(context,
               MILE_RESOURCE_MANAGER_LOG_CATEGORY,
               ELogType::FATAL,
               TEXT("Failed to create resource : ") + relativePath);
            SafeDelete(newResource);
         }

         return nullptr;
      }

      void ClearCache();

   private:
      ResourceCachePtr    m_cache;

   };
}