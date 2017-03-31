#pragma once
#include "MileManager.hpp"
#include "MileString.h"
#include <map>

namespace Mile
{
    /* Axis -> Has Scale, Action -> nothing **/
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

    struct MILE_API SKeyMappingSetting
    {
    public:
        SKeyMappingSetting( ) : 
            Key( EInputKey::IK_None )
        {

        }

        SKeyMappingSetting( EInputKey Key ) :
            Key( Key )
        {
        }

    public:
        EInputKey Key;

    };

    struct MILE_API SActionMappingSetting : public SKeyMappingSetting
    {
    public:
        SActionMappingSetting( ) :
            SKeyMappingSetting( )
        {
        }

        SActionMappingSetting( EInputKey Key ) :
            SKeyMappingSetting( Key )
        {
        }

    public:

    };

    struct MILE_API SAxisMappingSetting : public SKeyMappingSetting
    {
    public:
        SAxisMappingSetting( ) :
            Scale( 0.0f ), SKeyMappingSetting( )
        {
        }

        SAxisMappingSetting( EInputKey Key, float Scale ) :
            Scale( Scale ),
            SKeyMappingSetting( Key )
        {
        }

    public:
        float Scale;

    };

    using ActionEventDelegate = std::function<void( void )>;
    struct MILE_API SActionBindingSetting
    {
    public:
        SActionBindingSetting( )
        {
        }

        SActionBindingSetting( SActionMappingSetting MappingSetting, ActionEventDelegate Delegate ) :
            MappingSetting( MappingSetting ),
            Delegate( Delegate )
        {
        }

    public:
        SActionMappingSetting MappingSetting;
        ActionEventDelegate Delegate;

    };

    using AxisEventDelegate = std::function<void( float )>;
    struct MILE_API SAxisBindingSetting
    {
    public:
        SAxisBindingSetting( )
        {
        }

        SAxisBindingSetting( SAxisMappingSetting MappigSetting, AxisEventDelegate Delegate ) :
            MappingSetting( MappingSetting ),
            Delegate( Delegate )
        {
        }
        
    public:
        SAxisMappingSetting MappingSetting;
        AxisEventDelegate Delegate;

    };

    class MILE_API InputSystem : public Mile::Manager<InputSystem>
    {
        friend Manager;

    public:
        InputSystem( const InputSystem& ) = delete;
        InputSystem& operator=( const InputSystem& ) = delete;


        //bool RegisterDevice( InputDevice* );

        void MapAction( const MString& KeyName, SActionMappingSetting Setting );
        void UnmapAction( const MString& KeyName );
        void UnmapAllAction( );

        SActionMappingSetting GetActionMappingSetting( const MString& KeyName ) const;
        bool IsMappedAction( const MString& KeyName ) const;

        void MapAxis( const MString& KeyName, SAxisMappingSetting Setting );
        void UnmapAxis( const MString& KeyName );
        void UnmapAllAxis( );

        SAxisMappingSetting GetAxisMappingSetting( const MString& KeyName ) const;
        bool IsMappedAxis( const MString& KeyName ) const;

        void BindAction( const MString& KeyName, EActionInputEvent InputEvent, ActionEventDelegate Delegate );
        void UnbindAction( const MString& KeyName, EActionInputEvent InputEvent );
        void UnbindAllAction( EActionInputEvent InputEvent = EActionInputEvent::IE_Any );
        bool IsBindedAction( const MString& KeyName, EActionInputEvent InputEvent ) const;

        void BindAxis( const MString& KeyName, AxisEventDelegate Delegate );
        void UnbindAxis( const MString& KeyName );
        void UnbindAllAxis( );
        bool IsBindedAxis( const MString& KeyName ) const;

    private:
        InputSystem( ) : Mile::Manager<InputSystem>( MString( TEXT( "MainInputSystem" ) ) )
        {
        }

        ~InputSystem( );

    private:
        std::map<MString, SActionMappingSetting> ActionMappings;
        std::map<EInputKey, SActionBindingSetting> ActionPressedBind;
        std::map<EInputKey, SActionBindingSetting> ActionReleasedBind;

        std::map<MString, SAxisMappingSetting> AxisMappings;
        std::map<EInputKey, SAxisBindingSetting> AxisBind;
        
    };

}