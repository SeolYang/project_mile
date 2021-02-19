#include "Resource/Resource.h"
#include "Core/Context.h"
#include <cwctype>

namespace Mile
{
   unsigned int Resource::ResCount = 0;
   Resource::Resource(Context* context, const String& path, ResourceType resourceType) :
      m_context(context),
      m_path(path),
      m_name(GetFileNameFromPath(path)),
      m_ext(GetFileExtensionFromPath(m_path)),
      m_folder(GetFolderFromPath(path)),
      m_resourceType(resourceType),
      m_bIsInitialized(false),
      m_id(ResCount)
   {
      ++ResCount;
      std::transform(m_ext.begin(), m_ext.end(), m_ext.begin(), ::towlower);
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