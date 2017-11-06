#pragma once

#include "Core/SubSystem.h"
#include <fstream>
#include <string>

namespace Mile
{
    enum class ResourceType
    {
        RT_Unknown,
        RT_Texture1D,
        RT_Texture2D,
        RT_Texture3D,
        RT_Model,
        RT_Material,
        RT_Shader,
        RT_Audio,
        RT_Script,
        RT_PlainText
    };

    class MEAPI Resource
    {
    public:
        Resource( const String& path, ResourceType resourceType = ResourceType::RT_Unknown);
        virtual ~Resource( ) { }

        virtual bool Init( ) = 0;

        ResourceType GetType( ) const { return m_resourceType; }
        String GetName( ) const { return m_name; }
        String GetFolder( ) const { return m_folder; }
        String GetPath( ) const { return m_path; }

        static String GetFileNameFromPath( const String& filePath );
        static String GetFolderFromPath( const String& filePath );

        virtual bool Save( ) { return false; }

    protected:
        String     m_path;
        String     m_name;
        String     m_folder;
        ResourceType    m_resourceType;

    };

    using ResourcePtr = std::shared_ptr<Resource>;
    using WeakResourcePtr = std::weak_ptr<Resource>;
}