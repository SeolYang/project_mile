#include "SceneComponent.h"

namespace Mile
{
    void SceneComponent::AttachComponent( SceneComponent* Target )
    {
        if ( Target != nullptr )
        {
            if ( Target->IsRegistered( ) )
            {
                Target->SetParent( this );
            }

            Target->OnAttachPost( );
            Components.push_back( Target );
        }
    }

    bool SceneComponent::DetachComponent( const SceneComponent* Target )
    {
        if ( Target != nullptr )
        {
            for ( auto Itr = Components.begin( ); ( *Itr ) != Target; ++Itr )
            {
                ( *Itr )->OnDetachPost( );
                Components.erase( Itr );
                return true;
            }
        }

        return false;
    }

    void SceneComponent::SetParent( SceneComponent* NewParent )
    {
        DetachFromParent( );
        if ( NewParent != nullptr )
        {
            SetOwner( NewParent->GetOwner( ) );
            Parent = NewParent;
            Parent->AttachComponent( this );
        }
    }

    void SceneComponent::DetachFromParent( )
    {
        if ( Parent != nullptr )
        {
            Parent->DetachComponent( this );
        }
    }
}