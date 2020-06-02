#pragma once
#include "Resource/Resource.h"
#include "Resource/ResourceCache.h"

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
            if (m_cache->HasByPath(relativePath))
            {
               return GetByPath<Ty>(relativePath);
            }

            Context* context = GetContext();
            auto newResource = new Ty(context, relativePath);
            if (newResource->Init())
            {
               m_cache->Add(static_cast<ResourcePtr>(newResource));
               return newResource;
            }

            delete newResource;
            newResource = nullptr;
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
            auto res = Load<Ty>(relativePath);
            if (res != nullptr)
            {
               return res;
            }

            Context* context = GetContext();
            auto newResource = new Ty(context, relativePath);
            if (newResource->Save())
            {
               m_cache->Add(static_cast<Resource*>(newResource));
               return newResource;
            }

            delete newResource;
            newResource = nullptr;
         }

         return nullptr;
      }

      void ClearCache();

   private:
      ResourceCachePtr    m_cache;

   };
}