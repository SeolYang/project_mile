#include "Actor.h"
#include "ActorComponent.h"
#include "SceneComponent.h"

namespace Mile
{
    void Actor::PostTick( float DeltaTime )
    {
        //@todo: 컴포넌트가 추가되거나 삭제되면 우선순위대로 재 정렬한다.
    }
    void Actor::TickActor( float DeltaTime )
    {
        for ( auto Component : Components )
        {
            if ( Component->bIsTick )
            {
                Component->TickComponent( DeltaTime );
            }
        }
    }

    void Actor::AttachComponent( ActorComponent* Target )
    {
        if ( Target != nullptr )
        {
            if ( Target->IsRegistered( ) )
            {
                /** 이미 Owner가 있는경우엔 원래 Owner에서 Detach 시킨후 현재의 Owner로 변경시킨다. */
                Target->SetOwner( this );
            }

            Target->OnAttachPost( );
        }
    }

    bool Actor::DetachComponent( const ActorComponent* Target )
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

    void Actor::AttachRootComponent( SceneComponent* Target )
    {
        if ( Target != nullptr && RootComponent == nullptr )
        {
            AttachComponent( Target );
            RootComponent = Target;
        }
    }

    bool Actor::DetachRootComponent( )
    {
        if ( RootComponent != nullptr )
        {
            return DetachComponent( RootComponent );
        }

        return false;
    }

    SceneComponent* Actor::GetRootComponent( ) const
    {
        return RootComponent;
    }

}
