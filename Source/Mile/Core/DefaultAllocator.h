#pragma once
#include "Allocator.h"

/**
* 메모리 블럭을 할당해주는 기본 할당자입니다.
* @brief 범용적으로 작동하는 할당자입니다.
*/
class MILE_API DefaultAllocator : public MAllocator
{
public:
    virtual void* Allocate( uint64 Size ) override
    {
        return malloc( Size );
    }

    virtual void Deallocate( void* Target ) override
    {
        free( Target );
    }

    virtual void* Reallocate( void* Target, uint64 Size ) override
    {
        return realloc( Target, Size );
    }

    virtual void* AllocateAligned( uint64 SizeBytes, uint64 Alignment )
    {
        UNUSED_PARAM( SizeBytes );
        UNUSED_PARAM( Alignment );
        ASSERT_MSG( false, "Default Allocator is not support Aligned Allocate." );
    }

    virtual void DeallocateAligned( void* Target ) override
    {
        UNUSED_PARAM( Target );
        ASSERT_MSG( false, "Default Allocator is not support Aligned Deallocate." );
    }
};