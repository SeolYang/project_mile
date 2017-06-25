#pragma once
#include "MileObject.h"
#include "MileString.h"

namespace Mile
{
    template <class Ty>
    class MILE_API Manager : public Mile::Object
    {
    public:
        static Ty& Instance( Rumia::Allocator& Allocator )
        {
            if ( Manager::InstancePrivate == nullptr )
            {
                Manager::InstancePrivate = new Ty( Allocator );
            }
            
            ASSERT_MSG( ( Allocator != ( Manager<Ty>::InstancePrivate->GetAllocator( ) ) ), TEXT( "Must use same allocator" ) );
            return ( *Manager::InstancePrivate );
        }

        static bool DestroyInstance( )
        {
            if ( Manager::InstancePrivate != nullptr )
            {
                Mile::Object::DeleteObject( Manager::InstancePrivate );
                Manager::InstancePrivate = nullptr;
                return true;
            }

            return false;
        }

    protected:
        explicit Manager( Rumia::Allocator& Allocator, MString& Name ) :
            Mile::Object( Allocator, Name )
        {
        }

        virtual ~Manager( )
        {
        }

    private:
        static Ty* InstancePrivate;

    };

    template<typename Ty>
    Ty* Manager<Ty>::InstancePrivate = nullptr;

}