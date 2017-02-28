#pragma once
#include "Mile.h"

namespace Mile
{
    class Component;
    class MILE_API Object
    {
    public:
        Object( ){}

    private:
        bool bIsValid;

        std::vector<Component*> Components;

    };
}