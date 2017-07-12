#pragma once

#include "Rumia\Allocator.h"

#define MEAPI __declspec(dllexport)

namespace Mile
{
    using namespace Rumia;

    /**
    @brief  Release를 통해 메모리 할당 해제를 하는 인스턴스를 안전하게 해제시켜줍니다.
    */
    template <typename T>
    void SafeRelease( T& target )
    {
        if ( target != nullptr )
        {
            target->Release( );
            target = nullptr;
        }
    }
}