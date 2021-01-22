#pragma once
#include "Core/Helper.h"
#include "Resource/Resource.h"

namespace Mile
{
   class MEAPI ResourceCache
   {
   public:
      ResourceCache(Context* context);
      ~ResourceCache();

      void Add(ResourcePtr res);
      void Clear();

      ResourcePtr GetByPath(const String& path);
      ResourcePtr GetByName(const String& name);
      bool HasByPath(const String& path);
      bool HasByName(const String& name);
      bool IsValid(ResourcePtr target) const;

   private:
      Context* m_context;
      std::vector<ResourcePtr>      m_resources;

   };

   using ResourceCachePtr = std::unique_ptr<ResourceCache, std::function<void(ResourceCache*)>>;
}