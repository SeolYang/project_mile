#pragma once
#include "MileObject.h"
#include "Rumia\Array.h"
#include "Rumia\Pair.h"

namespace Mile
{
    class Component;
    class World;
    class Entity : public Mile::Object
    {
    protected:
        using ChildContainerType = Rumia::Array<Entity*>;
        using ComponentContainerType = Rumia::Array<Rumia::Pair<size_t, Component*>>;
        using ComponentPtrContainerType = Rumia::Array<Component*>;
        friend World;

    protected:
        Entity( Rumia::Allocator& Allocator, const MString& Name ) :
            World( nullptr ),
            Parent( nullptr ),
            Children( Allocator ),
            Components( Allocator ),
            bIsRegisteredAtWorld( false ),
            Object( Allocator, Name )
        {
        }

    public:
        virtual ~Entity( );

        Mile::World& GetWorld( )
        {
            ASSERT_MSG( this->World == nullptr, TEXT( "World does not exist!" ) );
            return ( *( this->World ) );
        }

        bool IsRegisteredAtWorld( ) const { return bIsRegisteredAtWorld; }

        Entity& GetParent( )
        {
            ASSERT_MSG( Parent == nullptr, TEXT( "Parent does not exist!" ) );
            return ( *Parent );
        }

        template <typename Ty>
        void AttachComponent( Ty* NewComponent )
        {
            NewComponent->SetOwner( this );
            Components.PushBack( Rumia::MakePair( typeid( Ty ).hash_code( ) ), NewComponent );
        }

        template <typename Ty>
        void DetachComponent( )
        {
            for ( auto CompPair : Components )
            {
                if ( CompPair.first == typeid( Ty ).hash_code( ) )
                {
                    DetachComponent( CompPair.second );
                    break;
                }
            }
        }

        void DetachComponent( uint64 TargetObjectID );

        void DetachComponent( Component* Target );

        void DetachComponent( ComponentContainerType::iterator TargetItr );

        template <typename Ty>
        void DetachComponents( )
        {
            for ( auto CompPair : Components )
            {
                if ( CompPair.first == typeid( Ty ).hash_code( ) )
                {
                    DetachComponent( CompPair.second );
                }
            }
        }

        template <typename Ty>
        Ty* GetComponent( )
        {
            for ( auto CompPair : Components )
            {
                if ( CompPair.first == typeid( Ty ).hash_code( ) )
                {
                    return static_cast< Ty* >( CompPair.second );
                }
            }
        }

        Component* GetComponent( uint64 TargetObjectID );

        template <typename Ty>
        Rumia::Array<Ty*>&& GetComponents( )
        {
            ComponentPtrContainerType Temp{ Allocator };
            for ( auto Comp : Components )
            {
                if ( typeid( Ty ).hash_code( ) == Comp.first )
                {
                    Temp.PushBack( Comp.second );
                }
            }

            return std::move( Temp );
        }

        void OnBegin( );
        void OnEnd( );

    private:
        void SetWorld( Mile::World* NewWorld );

    protected:
        Mile::World*            World;
        Entity*                 Parent;
        ChildContainerType      Children;
        ComponentContainerType  Components;
        bool                    bIsRegisteredAtWorld;

    };
}