#pragma once
#include "Core/CoreMinimal.h"

namespace Mile
{
   class Context;
   class Resource;
   class MEAPI ResourceCache
   {
   public:
      ResourceCache(Context* context);
      ~ResourceCache();

      void Add(Resource* res);
      void Clear();
      void Remove(Resource* resPtr);

      Resource* GetByPath(const String& path) const;
      Resource* GetByName(const String& name) const;
      bool HasByPath(const String& path) const;
      bool HasByName(const String& name) const;
      bool IsValid(Resource* target) const;

   private:
      Context* m_context;
      mutable std::mutex m_mutex;
      std::vector<Resource*> m_resources;

   };

   using ResourceCachePtr = std::unique_ptr<ResourceCache, std::function<void(ResourceCache*)>>;
}