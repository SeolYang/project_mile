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

        virtual bool Init( ) = 0;
        virtual void Update( ) { }
        virtual void DeInit( ) { }

    protected:
        Context* m_context;

    };
}