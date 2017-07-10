#pragma once

#include "Helper.h"

namespace Mile
{
    class MEAPI SubSystem
    {
        friend class Context;
    public:
        SubSystem( Context& context );
        virtual ~SubSystem( ) { }

        virtual bool Initialize( ) = 0;

    protected:
        Context& m_context;

    };
}