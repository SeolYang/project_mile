#include "MileInputSystem.h"

namespace Mile
{
    InputSystem* InputSystem::Instance = nullptr;

    InputSystem::~InputSystem( )
    {
        UnmapAllAction( );
        UnmapAllAxis( );
    }

    InputSystem& InputSystem::GetInstance( )
    {
        if ( Instance == nullptr )
        {
            Instance = new InputSystem( );
        }

        return ( *Instance );
    }

    bool InputSystem::DestroyInstance( )
    {
        if ( Instance != nullptr )
        {
            delete Instance;
            Instance = nullptr;
            return true;
        }
        
        return true;
    }

    void InputSystem::MapAction( const MString& KeyName, SActionMappingSetting Setting )
    {
        ActionMappings[ KeyName ] = Setting;
    }

    void InputSystem::UnmapAction( const MString& KeyName )
    {
        ActionMappings.erase( KeyName );
    }

    void InputSystem::UnmapAllAction( )
    {
        ActionMappings.clear( );
        UnbindAllAction( );
    }

    SActionMappingSetting InputSystem::GetActionMappingSetting( const MString& KeyName ) const
    {
        if ( IsMappedAction( KeyName ) )
        {
            return ( *( ActionMappings.find( KeyName ) ) ).second;
        }

        return SActionMappingSetting( );
    }

    bool InputSystem::IsMappedAction( const MString& KeyName ) const
    {
        return ( ( ActionMappings.find( KeyName ) != ActionMappings.end( ) ) );
    }

    void InputSystem::MapAxis( const MString& KeyName, SAxisMappingSetting Setting )
    {
        AxisMappings[ KeyName ] = Setting;
    }

    void InputSystem::UnmapAxis( const MString& KeyName )
    {
        AxisMappings.erase( KeyName );
    }

    void InputSystem::UnmapAllAxis( )
    {
        AxisMappings.clear( );
        UnbindAllAxis( );
    }

    SAxisMappingSetting InputSystem::GetAxisMappingSetting( const MString& KeyName ) const
    {
        if ( IsMappedAxis( KeyName ) )
        {
            return ( *( AxisMappings.find( KeyName ) ) ).second;
        }

        return SAxisMappingSetting( );
    }

    bool InputSystem::IsMappedAxis( const MString& KeyName ) const
    {
        return ( AxisMappings.find( KeyName ) != AxisMappings.end( ) );
    }

    void InputSystem::BindAction( const MString& KeyName, EActionInputEvent InputEvent, ActionEventDelegate Delegate )
    {
        if ( IsMappedAction( KeyName ) )
        {
            SActionMappingSetting MappingSetting = GetActionMappingSetting( KeyName );
            SActionBindingSetting BindingSetting = SActionBindingSetting( MappingSetting, Delegate );

            switch ( InputEvent )
            {
            case EActionInputEvent::IE_Pressed:
                ActionPressedBind[ MappingSetting.Key ] = BindingSetting;
                break;

            case EActionInputEvent::IE_Released:
                ActionReleasedBind[ MappingSetting.Key ] = BindingSetting;
                break;

            case EActionInputEvent::IE_Any:
                ActionPressedBind[ MappingSetting.Key ] = BindingSetting;
                ActionReleasedBind[ MappingSetting.Key ] = BindingSetting;
                break;
            }
        }
    }

    void InputSystem::UnbindAction( const MString& KeyName, EActionInputEvent InputEvent )
    {
        if ( IsMappedAction( KeyName ) )
        {
            SActionMappingSetting MappingSetting = GetActionMappingSetting( KeyName );
            switch ( InputEvent )
            {
            case EActionInputEvent::IE_Pressed:
                ActionPressedBind.erase( MappingSetting.Key );
                break;

            case EActionInputEvent::IE_Released:
                ActionReleasedBind.erase( MappingSetting.Key );
                break;

            case EActionInputEvent::IE_Any:
                ActionPressedBind.erase( MappingSetting.Key );
                ActionReleasedBind.erase( MappingSetting.Key );
                break;
            }
        }
    }

    void InputSystem::UnbindAllAction( EActionInputEvent InputEvent )
    {
        switch ( InputEvent )
        {
        case EActionInputEvent::IE_Pressed:
            ActionPressedBind.clear( );
            break;

        case EActionInputEvent::IE_Released:
            ActionReleasedBind.clear( );
            break;

        default:
        case EActionInputEvent::IE_Any:
            ActionPressedBind.clear( );
            ActionReleasedBind.clear( );
            break;
        }

    }

    bool InputSystem::IsBindedAction( const MString& KeyName, EActionInputEvent InputEvent ) const
    {
        if ( IsMappedAction( KeyName ) )
        {
            SActionMappingSetting MappingSetting = GetActionMappingSetting( KeyName );
            switch ( InputEvent )
            {
            case EActionInputEvent::IE_Pressed:
                return ( ActionPressedBind.find( MappingSetting.Key ) != ActionPressedBind.end( ) );

            case EActionInputEvent::IE_Released:
                return ( ActionReleasedBind.find( MappingSetting.Key ) != ActionPressedBind.end( ) );

            case EActionInputEvent::IE_Any:
                bool bIsPressedBind = ( ActionPressedBind.find( MappingSetting.Key ) != ActionPressedBind.end( ) );
                bool bIsReleasedBind = ( ActionReleasedBind.find( MappingSetting.Key ) != ActionPressedBind.end( ) );
                return ( bIsPressedBind && bIsReleasedBind );
            }
        }

        return false;
    }

    void InputSystem::BindAxis( const MString& KeyName, AxisEventDelegate Delegate )
    {
        if ( IsMappedAxis( KeyName ) )
        {
            SAxisMappingSetting MappingSetting = GetAxisMappingSetting( KeyName );
            AxisBind[ MappingSetting.Key ] = SAxisBindingSetting( MappingSetting, Delegate );
        }
    }

    void InputSystem::UnbindAxis( const MString& KeyName )
    {
        if ( IsMappedAxis( KeyName ) )
        {
            SAxisMappingSetting MappingSetting = GetAxisMappingSetting( KeyName );
            AxisBind.erase( MappingSetting.Key );
        }
    }

    void InputSystem::UnbindAllAxis( )
    {
        AxisBind.clear( );
    }

    bool InputSystem::IsBindedAxis( const MString& KeyName ) const
    {
        if ( IsMappedAxis( KeyName ) )
        {
            SAxisMappingSetting MappingSetting = GetAxisMappingSetting( KeyName );
            return ( AxisBind.find( MappingSetting.Key ) != AxisBind.end( ) );
        }

        return false;
    }
}