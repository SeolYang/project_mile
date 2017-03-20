#pragma once
#include "MileObject.h"
#include "Transform.h"
#include <iostream>

namespace Mile
{
    class ActorComponent;
    class SceneComponent;
    class MileWorld;
    class MILE_API Actor : public Mile::Object
    {
        using ComponentList = std::vector<ActorComponent*>;
    public:
        Actor( const MString& NewName, MileWorld& NewWorld ) :
            World( NewWorld ),
            bIsChangedAtComponents( true ),
            ParentPrivate( nullptr ),
            Object( NewName )
        {
        }

        virtual void OnGameBegin( ){ }
        virtual void OnGameEnd( ){ }

        void AttachComponent( ActorComponent* Component );
        void DetachComponent( ActorComponent* Component );

        bool AttachToComponent( SceneComponent* Parent );
        void DetachFromComponent( );

        void AttachToActor( Actor* Parent );
        void DetachFromParent( );

        bool SetRootComponent( SceneComponent* NewRootComponent );
        FORCEINLINE SceneComponent* GetRootComponent( ) const { return RootComponent; }
        FORCEINLINE SceneComponent* GetDefaultAttachComponent( ) const { return RootComponent; }

        FORCEINLINE Actor* GetParent( ) { return ParentPrivate; }

        Vector GetPosition( ETransformRelation Relation = ETransformRelation::Relative );
        Vector GetRotation( ETransformRelation Relation = ETransformRelation::Relative );
        Vector GetScale( ETransformRelation Relation = ETransformRelation::Relative );

        FORCEINLINE MileWorld& GetWorld( ) { return World; }

    private:
        MileWorld&          World;
        ComponentList       Components;
        SceneComponent*     RootComponent;
        Actor*              ParentPrivate;
        
    public:
        bool                bIsChangedAtComponents;

    };
}