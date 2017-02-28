#pragma once
#include "ActorComponent.h"
#include "Transform.h"

namespace Mile
{
    /**
    * SceneComponent는 ActorComponent에 Transform 정보를 확장하고 서로 붙일수있는 Component 이다.
    */
    class MILE_API SceneComponent : public ActorComponent
    {
        using ComponentList = std::vector<SceneComponent*>;
    public:
        SceneComponent( const MString& NewName, Actor* NewOwner ) :
            ActorComponent( NewName, NewOwner )
        {
        }

        void SetTransform( const Transform& NewTransform )
        {
            Transform = NewTransform;
        }

        Transform& GetTransform( )
        {
            return Transform;
        }

        void AttachComponent( SceneComponent* Target );
        bool DetachComponent( const SceneComponent* Target );

        void SetParent( SceneComponent* NewParent );
        SceneComponent* GetParent( ) const
        {
            return Parent;
        }

    protected:
        virtual void OnDetachPost( ) override
        {
            ActorComponent::OnDetachPost( );
            Parent = nullptr;
        }

    private:
        void DetachFromParent( );

    private:
        Transform Transform;
        SceneComponent* Parent;
        ComponentList Components;

    };
}