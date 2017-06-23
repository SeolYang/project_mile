#include "InputSystem.h"

namespace Mile
{
    InputSystem::~InputSystem( )
    {
        UnmapAllAction( );
        UnmapAllAxis( );
    }

    void InputSystem::MapAction( const MString& MappingKey, SActionMappingProperty Property )
    {
        ActionMap[ MappingKey ].first.push_back( Property );
    }

    void InputSystem::UnmapAction( const MString& MappingKey )
    {
        ActionMap.erase( MappingKey );
    }

    void InputSystem::UnmapActionKey( const MString& MappingKey, EInputKey Key )
    {
        ActionPropertyListType PropertyList = ActionMap[ MappingKey ].first;
        auto FoundItr = std::find_if( PropertyList.begin( ), PropertyList.end( ),
                                      [ Key ]( SActionMappingProperty Itr )->bool { return ( Itr.Key == Key ); } );
        PropertyList.erase( FoundItr );
    }

    void InputSystem::UnmapAllAction( )
    {
        ActionMap.clear( );
    }

    bool InputSystem::IsMappedAction( const MString& MappingKey ) const
    {
        return ( ActionMap.find( MappingKey ) != ActionMap.end( ) );
    }

    void InputSystem::MapAxis( const MString& MappingKey, SAxisMappingProperty Property )
    {
        AxisMap[ MappingKey ].first.push_back( Property );
    }

    void InputSystem::UnmapAxis( const MString& MappingKey )
    {
        AxisMap.erase( MappingKey );
    }

    void InputSystem::UnmapAxisKey( const MString& MappingKey, EInputKey Key )
    {
        AxisPropertyListType PropertyList = AxisMap[ MappingKey ].first;
        auto FoundItr = std::find_if( PropertyList.begin( ), PropertyList.end( ),
                                      [ Key ]( SAxisMappingProperty Itr )->bool { return ( Itr.Key == Key ); } );
        PropertyList.erase( FoundItr );
    }

    void InputSystem::UnmapAllAxis( )
    {
        AxisMap.clear( );
    }

    bool InputSystem::IsMappedAxis( const MString& MappingKey ) const
    {
        return ( AxisMap.find( MappingKey ) != AxisMap.end( ) );
    }

    void InputSystem::BindAction( const MString& MappingKey, EActionInputEvent InputEvent, ActionEventDelegate Delegate )
    {
        if ( IsMappedAction( MappingKey ) )
        {
            // ActionMap has pair of delegate.
            // First delegate for Pressed Event
            // Second delegate for Released Event
            switch ( InputEvent )
            {
            case EActionInputEvent::IE_Pressed:
                ActionMap[ MappingKey ].second.first = Delegate;
                break;

            case EActionInputEvent::IE_Released:
                ActionMap[ MappingKey ].second.second = Delegate;
                break;

            case EActionInputEvent::IE_Any:
                ActionMap[ MappingKey ].second.first = ActionMap[ MappingKey ].second.second = Delegate;
                break;
            }
        }
    }

    void InputSystem::UnbindAction( const MString& MappingKey, EActionInputEvent InputEvent )
    {
        if ( IsMappedAction( MappingKey ) )
        {
            switch ( InputEvent )
            {
            case EActionInputEvent::IE_Pressed:
                ActionMap[ MappingKey ].second.first = nullptr;
                break;

            case EActionInputEvent::IE_Released:
                ActionMap[ MappingKey ].second.second = nullptr;
                break;

            case EActionInputEvent::IE_Any:
                ActionMap[ MappingKey ].second.second = nullptr;
                break;
            }
        }
    }

    void InputSystem::UnbindAllAction( EActionInputEvent InputEvent )
    {
        switch ( InputEvent )
        {
        case EActionInputEvent::IE_Pressed:
            for ( auto& MappedAction : ActionMap )
            {
                ( MappedAction.second.second ).first = nullptr;
            }
            break;

        case EActionInputEvent::IE_Released:
            for ( auto& MappedAction : ActionMap )
            {
                ( MappedAction.second.second ).second = nullptr;
            }
            break;

        default:
        case EActionInputEvent::IE_Any:
            for ( auto& MappedAction : ActionMap )
            {
                ( MappedAction.second.second ).first = nullptr;
                ( MappedAction.second.second ).second = nullptr;
            }
            break;
        }

    }

    bool InputSystem::IsBindedAction( const MString& MappingKey, EActionInputEvent InputEvent ) const
    {
        if ( IsMappedAction( MappingKey ) )
        {
            switch ( InputEvent )
            {
            case EActionInputEvent::IE_Pressed:
                return ( ( ActionMap.find( MappingKey )->second.second ).first != nullptr );

            case EActionInputEvent::IE_Released:
                return ( ( ActionMap.find( MappingKey )->second.second ).second != nullptr );

            default:
            case EActionInputEvent::IE_Any:
                bool bIsPressedBinded = ( ( ActionMap.find( MappingKey )->second.second ).first != nullptr );
                bool bIsReleasedBinded = ( ( ActionMap.find( MappingKey )->second.second ).second != nullptr );
                return ( bIsPressedBinded && bIsReleasedBinded );
            }
        }

        return false;
    }

    void InputSystem::BindAxis( const MString& MappingKey, AxisEventDelegate Delegate )
    {
        if ( IsMappedAxis( MappingKey ) )
        {
            AxisMap[ MappingKey ].second = Delegate;
        }
    }

    void InputSystem::UnbindAxis( const MString& MappingKey )
    {
        if ( IsMappedAxis( MappingKey ) )
        {
            AxisMap[ MappingKey ].second = nullptr;
        }
    }

    void InputSystem::UnbindAllAxis( )
    {
        for ( auto& MappedAxis : AxisMap )
        {
            ( MappedAxis.second ).second = nullptr;
        }
    }

    bool InputSystem::IsBindedAxis( const MString& MappingKey ) const
    {
        if ( IsMappedAxis( MappingKey ) )
        {
            return ( ( AxisMap.find( MappingKey )->second ).second != nullptr );
        }

        return false;
    }
}