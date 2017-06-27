#pragma once
#include "MileObject.h"
#include "Rumia/Array.h"

namespace Mile
{
    class Entity;
    class MILE_API World final : public Mile::Object
    {
    protected:
        World( Rumia::Allocator& Allocator, const MString& Name ) :
            Entities( Allocator ),
            Object( Allocator, Name )
        {

        }

    public:
        virtual ~World( );

        void RegisterEntity( Entity* NewEntity );
        void UnRegisterEntity( Entity* TargetEntity );
        void UnRegisterAllEntities( );

        void OnBegin( );
        void OnEnd( );

    protected:
        Rumia::Array<Entity*> Entities;

    };
}