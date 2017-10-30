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
        Resource( const std::string& path, ResourceType resourceType = ResourceType::RT_Unknown);
        virtual ~Resource( ) { }

        virtual bool Initialize( ) = 0;

        ResourceType GetType( ) const { return m_resourceType; }
        std::string GetName( ) const { return m_name; }
        std::string GetFolder( ) const { return m_folder; }
        std::string GetPath( ) const { return m_path; }

        static std::string GetFileNameFromPath( const std::string& filePath );
        static std::string GetFolderFromPath( const std::string& filePath );

        virtual bool Save( ) { return false; }

    protected:
        std::string     m_path;
        std::string     m_name;
        std::string     m_folder;
        ResourceType    m_resourceType;

    };

    using ResourcePtr = std::shared_ptr<Resource>;
    using WeakResourcePtr = std::weak_ptr<Resource>;
}