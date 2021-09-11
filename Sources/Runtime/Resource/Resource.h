#pragma once
#include "Core/SubSystem.h"

namespace Mile
{
   enum class ResourceType
   {
      Unknown,
      Texture1D,
      Texture2D,
      Texture3D,
      RenderTexture,
      Model,
      Material,
      Shader,
      Audio,
      Script,
      PlainText
   };

   class ResourceManager;
   class MEAPI Resource
   {
   public:
      Resource(ResourceManager* resMng, ResourceType resourceType = ResourceType::Unknown);
      virtual ~Resource() { }

      virtual void LoadMetafile() { }
      virtual void SaveMetafile() { }
      virtual bool Init(const String& path)
      {
         if (m_resMng != nullptr && !m_bIsInitialized)
         {
            SetPath(path);
            LoadMetafile();
            return true;
         }

         return false;
      }

      virtual bool SaveTo(const String& filePath)
      {
         SetPath(filePath);
         SaveMetafile();
         return true;
      }
      bool Save() { return SaveTo(this->m_path); }

      virtual json Serialize() const { return json(); }
      virtual void DeSerialize(const json& jsonData) { /* Nothing to do */ }

      ResourceType GetType() const { return m_resourceType; }
      String GetName() const { return m_name; }
      String GetFolder() const { return m_folder; }
      String GetPath() const { return m_path; }
      String GetRelativePath() const;
      String GetExt() const { return m_ext; }
      String GetMetaPath() const { String path = m_folder; return (path.append(m_name).append(TEXT(".meta"))); }
      unsigned int GetID() const { return m_id; }

      static String GetFileNameFromPath(const String& filePath, bool includeExt = false);
      static String GetFolderFromPath(const String& filePath);
      static String GetFileExtensionFromPath(const String& filePath);

   protected:
      void SucceedInit() { m_bIsInitialized = true; }

   private:
      void SetPath(const String& path)
      {
         m_path = path;
         m_name = GetFileNameFromPath(path);
         m_ext = GetFileExtensionFromPath(m_path);
         std::transform(m_ext.begin(), m_ext.end(), m_ext.begin(), ::towlower);
         m_folder = GetFolderFromPath(path);
      }

   protected:
      ResourceManager* m_resMng;
      String     m_path;
      String     m_name;
      String     m_folder;
      String     m_ext;
      ResourceType    m_resourceType;
      unsigned int m_id;

      bool       m_bIsInitialized;

   private:
      static unsigned int ResCount;

      friend ResourceManager;

   };

   using ResourcePtr = Resource*;
}