#pragma once
#include "MileObject.h"

namespace Mile
{
    class Entity;
    class Component : public Mile::Object
    {
        friend Entity;
    protected:
        Component( Rumia::Allocator& Allocator, const MString& Name ) :
            Owner( nullptr ),
            Mile::Object( Allocator, Name )
        {
        }
        
    public:
        Entity& GetOwner( ) 
        { 
            ASSERT_MSG( Owner == nullptr, TEXT( "Owner does not exist!" ) );
            return ( *Owner ); 
        }

    private:
        void SetOwner( Entity* NewOwner );

    protected:
        Entity* Owner;

    };
}