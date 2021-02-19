#include "Resource/ResourceCache.h"
#include "Resource/Resource.h"

namespace Mile
{
   ResourceCache::ResourceCache(Context* context) :
      m_context(context),
      m_resources()
   {
   }

   ResourceCache::~ResourceCache()
   {
      Clear();
   }

   void ResourceCache::Add(Resource* res)
   {
      if (res != nullptr)
      {
         std::lock_guard<std::mutex> lock(m_mutex);
         m_resources.push_back(res);
      }
   }

   void ResourceCache::Clear()
   {
      std::lock_guard<std::mutex> lock(m_mutex);
      for (auto resource : m_resources)
      {
         SafeDelete(resource);
      }

      m_resources.clear();
   }

   void ResourceCache::Remove(Resource* resPtr)
   {
      std::lock_guard<std::mutex> lock(m_mutex);
      for (auto resItr = m_resources.begin(); resItr != m_resources.end(); ++resItr)
      {
         if (resPtr == (*resItr))
         {
            m_resources.erase(resItr);
            SafeDelete(resPtr);
            return;
         }
      }
   }

   Resource* ResourceCache::GetByPath(const String& path) const
   {
      std::lock_guard<std::mutex> lock(m_mutex);
      for (auto res : m_resources)
      {
         if (res->GetPath() == path)
         {
            return res;
         }
      }

      return nullptr;
   }

   Resource* ResourceCache::GetByName(const String& name) const
   {
      std::lock_guard<std::mutex> lock(m_mutex);
      for (auto res : m_resources)
      {
         if (res->GetName() == name)
         {
            return res;
         }
      }

      return nullptr;
   }

   bool ResourceCache::HasByPath(const String& name) const
   {
      std::lock_guard<std::mutex> lock(m_mutex);
      for (auto res : m_resources)
      {
         if (res->GetPath() == name)
         {
            return true;
         }
      }

      return false;
   }

   bool ResourceCache::HasByName(const String& name) const
   {
      std::lock_guard<std::mutex> lock(m_mutex);
      for (auto res : m_resources)
      {
         if (res->GetName() == name)
         {
            return true;
         }
      }

      return false;
   }

   bool ResourceCache::IsValid(Resource* target) const
   {
      std::lock_guard<std::mutex> lock(m_mutex);
      for (auto resource : m_resources)
      {
         if (resource == target)
         {
            return true;
         }
      }

      return false;
   }

}