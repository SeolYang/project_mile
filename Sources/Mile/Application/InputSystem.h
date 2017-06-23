#pragma once
#include "MileManager.hpp"
#include "MileString.h"
#include "Rumia/Array.h"
#include <vector>
#include <map>

// @TODO: Perfectly replace std::vector to Rumia::Array
// 하나의 Mapping Key는 여러개의 Input Key를 Mapping 할 수 있음.
// 하나의 Mapping Key는 하나의 Delegate를 Bind 할 수 있음.
// Input Key는 이미 Mapping 되어 있어도 다른 Mapping Key에 Mapping 시킬 수 잇음.
// Input Key-Mapping Key Container, Mapping Key-Delegate Container
namespace Mile
{
    enum class EActionInputEvent
    {
        IE_Pressed,
        IE_Released,
        IE_Any
    };

    enum class EInputKey
    {
        IK_None
    };

    struct MILE_API SKeyMappingProperty
    {
    public:
        SKeyMappingProperty( ) :
            Key( EInputKey::IK_None )
        {

        }

        SKeyMappingProperty( EInputKey Key ) :
            Key( Key )
        {
        }

    public:
        EInputKey Key;

    };

    struct MILE_API SActionMappingProperty : public SKeyMappingProperty
    {
    public:
        SActionMappingProperty( ) :
            SKeyMappingProperty( )
        {
        }

        SActionMappingProperty( EInputKey Key ) :
            SKeyMappingProperty( Key )
        {
        }

    public:

    };

    struct MILE_API SAxisMappingProperty : public SKeyMappingProperty
    {
    public:
        SAxisMappingProperty( ) :
            Scale( 0.0f ), SKeyMappingProperty( )
        {
        }

        SAxisMappingProperty( EInputKey Key, float Scale ) :
            Scale( Scale ),
            SKeyMappingProperty( Key )
        {
        }

    public:
        float Scale;

    };

    using ActionEventDelegate = std::function<void( void )>;
    using AxisEventDelegate = std::function<void( float )>;

    class MILE_API InputSystem : public Mile::Manager<InputSystem>
    {
        friend Manager;

    public:
        InputSystem( const InputSystem& ) = delete;
        InputSystem& operator=( const InputSystem& ) = delete;

        //bool RegisterDevice( InputDevice* );

        void MapAction( const MString& MappingKey, SActionMappingProperty Property );
        void UnmapAction( const MString& MappingKey );
        void UnmapActionKey( const MString& MappingKey, EInputKey Key );
        void UnmapAllAction( );
        bool IsMappedAction( const MString& MappingKey ) const;

        void MapAxis( const MString& MappingKey, SAxisMappingProperty Property );
        void UnmapAxis( const MString& MappingKey );
        void UnmapAxisKey( const MString& MappingKey, EInputKey Key );
        void UnmapAllAxis( );
        bool IsMappedAxis( const MString& MappingKey ) const;

        void BindAction( const MString& MappingKey, EActionInputEvent InputEvent, ActionEventDelegate Delegate );
        void UnbindAction( const MString& MappingKey, EActionInputEvent InputEvent );
        void UnbindAllAction( EActionInputEvent InputEvent = EActionInputEvent::IE_Any );
        bool IsBindedAction( const MString& MappingKey, EActionInputEvent InputEvent ) const;

        void BindAxis( const MString& MappingKey, AxisEventDelegate Delegate );
        void UnbindAxis( const MString& MappingKey );
        void UnbindAllAxis( );
        bool IsBindedAxis( const MString& MappingKey ) const;

    private:
        InputSystem( ) : Mile::Manager<InputSystem>( MString( TEXT( "MainInputSystem" ) ) )
        {
        }

        ~InputSystem( );

    private:
        // <Mapping Key, pair<vector<ActionKeyProperty>, pair<PressedDelegate, ReleasedDelegate>>>
        // <Mapping Key, pair<vector<AxisKeyProperty>, Delegate>>
        using ActionPropertyListType = std::vector<SActionMappingProperty>;
        using AxisPropertyListType = std::vector<SAxisMappingProperty>;
        using ActionMapType = std::map<MString, std::pair<ActionPropertyListType, std::pair<ActionEventDelegate, ActionEventDelegate>>>;
        using AxisMapType = std::map<MString, std::pair<AxisPropertyListType, AxisEventDelegate>>;
        ActionMapType ActionMap;
        AxisMapType AxisMap;

    };

}