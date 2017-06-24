#pragma once
#include "MileObject.h"
#include "MileString.h"

namespace Mile
{
    template <class Ty>
    class MILE_API Manager
    {
    public:
        static Ty& Instance( )
        {
            if ( Manager::InstancePrivate == nullptr )
            {
                Manager::InstancePrivate = new Ty( );
            }

            return ( *Manager::InstancePrivate );
        }

        static bool DestroyInstance( )
        {
            if ( Manager::InstancePrivate != nullptr )
            {
                delete ( Manager::InstancePrivate );
                Manager::InstancePrivate = nullptr;
                return true;
            }

            return false;
        }

    protected:
        explicit Manager( )
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