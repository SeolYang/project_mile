#pragma once

#include "Core/SubSystem.h"

namespace Mile
{
    enum class ResourceType
    {
        RT_Unknown,
        RT_Texture,
        RT_Model,
        RT_Material,
        RT_Shader,
        RT_Audio,
        RT_Script
    };

    class MEAPI Resource
    {
    public:
        Resource( Context* context, const std::string& path, const std::string& name, const std::string& folder, ResourceType resourceType = ResourceType::RT_Unknown);
        virtual ~Resource( ) { }

        virtual bool Initialize( ) = 0;

        ResourceType GetType( ) const { return m_resourceType; }
        std::string GetName( ) const { return m_name; }
        std::string GetFolder( ) const { return m_folder; }
        std::string GetPath( ) const { return m_path; }

        static std::string GetFileNameFromPath( Allocator& allocator, const std::string& filePath );
        static std::string GetFolderFromPath( Allocator& allocator, const std::string& filePath );

    protected:
        Context*        m_context;
        std::string     m_path;
        std::string     m_name;
        std::string     m_folder;
        ResourceType    m_resourceType;

    };

    using ResourcePtr = std::shared_ptr<Resource>;
    using WeakResourcePtr = std::weak_ptr<Resource>;
}