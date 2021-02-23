#include "Resource/Resource.h"
#include "Core/Context.h"
#include <cwctype>

namespace Mile
{
   unsigned int Resource::ResCount = 0;
   Resource::Resource(ResourceManager* resMng, ResourceType resourceType) :
      m_resMng(resMng),
      m_bIsInitialized(false),
      m_resourceType(resourceType),
      m_id(ResCount)
   {
      ++ResCount;
   }

   String Resource::GetFileNameFromPath(const String& filePath, bool includeExt)
   {
      auto splitPath = std::move(SplitStr(filePath, '/'));
      if (splitPath.size() == 1)
      {
         splitPath = std::move(SplitStr(filePath, '\\'));
      }

      if (includeExt)
      {
         return splitPath[splitPath.size() - 1];
      }

      auto splitFileName = SplitStr(splitPath[splitPath.size() - 1], '.');
      return splitFileName[0];
   }

   String Resource::GetFolderFromPath(const String & filePath)
   {
      auto splitPath = std::move(SplitStr(filePath, '/'));
      if (splitPath.size() == 1)
      {
         splitPath = std::move(SplitStr(filePath, '\\'));
      }
      splitPath.pop_back();

      return CombineStr(splitPath, TEXT("/"));
   }

   String Resource::GetFileExtensionFromPath(const String & filePath)
   {
      auto name = GetFileNameFromPath(filePath, true);
      auto splitName = SplitStr(name, '.');
      return splitName[splitName.size() - 1];
   }
}