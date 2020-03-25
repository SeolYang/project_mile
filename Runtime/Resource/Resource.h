#pragma once
#include "Core/SubSystem.h"
#include <fstream>
#include <string>

namespace Mile
{
   enum class ResourceType
   {
      Unknown,
      Texture1D,
      Texture2D,
      Texture3D,
      Model,
      Material,
      Shader,
      Audio,
      Script,
      PlainText
   };

   class MEAPI Resource
   {
   public:
      Resource(Context* context, const String& path, ResourceType resourceType = ResourceType::Unknown);
      virtual ~Resource() { }

      virtual bool LoadMetafile() { return false; }
      virtual bool Init() = 0;
      virtual bool SaveTo(const String& filePath) { return false; }
      virtual bool Save() { return SaveTo(this->m_path); }

      virtual json Serialize() const { return json(); }
      virtual void DeSerialize(const json& jsonData) { /* Nothing to do */ }

      ResourceType GetType() const { return m_resourceType; }
      String GetName() const { return m_name; }
      String GetFolder() const { return m_folder; }
      String GetPath() const { return m_path; }
      String GetExt() const { return m_ext; }
      unsigned int GetID() const { return m_id; }

      static String GetFileNameFromPath(const String& filePath, bool includeExt = false);
      static String GetFolderFromPath(const String& filePath);
      static String GetFileExtensionFromPath(const String& filePath);

   protected:
      Context* m_context;
      String     m_path;
      String     m_name;
      String     m_folder;
      String     m_ext;
      ResourceType    m_resourceType;
      unsigned int m_id;

      bool       m_bIsInitialized;

   private:
      static unsigned int ResCount;

   };

   using ResourcePtr = Resource *;
}