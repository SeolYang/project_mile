#pragma once

#include "Helper.h"

namespace Mile
{
    class Context;
    class MEAPI SubSystem
    {
    public:
        SubSystem( Context* context );
        virtual ~SubSystem( ) { }

        virtual bool Initialize( ) = 0;
        virtual void Update( ) { }

    protected:
        Context* m_context;

    };
}