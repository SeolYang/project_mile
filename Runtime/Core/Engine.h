#pragma once

#include "SubSystem.h"

namespace Mile
{
    class MEAPI Engine : public SubSystem
    {
    public:
        Engine( Context* context );
        virtual ~Engine( ) { ShutDown( ); }

        virtual bool Initialize( ) override;

        void Update( );
        void PostUpdate( );

        void ShutDown( );

    };
}