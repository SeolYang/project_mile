#pragma once
#include "Allocator.h"

namespace Rumia
{
    class DefaultAllocator : public Allocator
    {
    public:
        ~DefaultAllocator( ) { }

        virtual void* Allocate( size_t size )
        {
            return std::malloc( size );
        }

        virtual void* AllocateAligned( size_t size, size_t align )
        {
            return std::malloc( size );
        }

        virtual void Deallocate( void* ptr )
        {
            std::free( ptr );
        }
        
        virtual void DeallocateAligned( void* ptr, size_t size, size_t align ) override
        {
            std::free( ptr );
        }

    };
}