#include "SceneComponent.h"

namespace Mile
{
    bool SceneComponent::AttachTo( SceneComponent* NewParent )
    {
        bool bIsValidAttachment =
            NewParent != nullptr && ParentPrivate != NewParent && NewParent->GetParent( ) != this;
        if ( bIsValidAttachment )
        {
            DetachFromComponent( );
            ParentPrivate = NewParent;
            ParentPrivate->AddAttachedComponent( this );
            std::wcout << TEXT( "Scene Component has attached to " ) << ( std::wstring )ParentPrivate->GetName( ) << TEXT( ".\n" );
            return true;
        }
        else
        {
            std::wcout << TEXT( "Is not able to attach." ) << std::endl;
        }

        return false;
    }

    void SceneComponent::DetachFromComponent( )
    {
        bool bIsValidDetachment =
            ParentPrivate != nullptr;
        if ( bIsValidDetachment )
        {
            if ( ParentPrivate->RemoveAttachedComponent( this ) )
            {
                std::wcout << TEXT( "Scene Component has detached from " ) << ( std::wstring )ParentPrivate->GetName( ) << TEXT( ".\n" );
                ParentPrivate = nullptr;
            }
        }
    }

    void SceneComponent::AddAttachedComponent( SceneComponent * Component )
    {
        Components.push_back( Component );
    }

    bool SceneComponent::RemoveAttachedComponent( SceneComponent * Component )
    {
        for ( auto FoundComponent = Components.begin( );
            ( *FoundComponent ) != this;
            ++FoundComponent )
        {
            Components.erase( FoundComponent );
            return true;
        }
    }

}