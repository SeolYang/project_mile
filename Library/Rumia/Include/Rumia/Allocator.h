#pragma once
#include <cstdlib>
#include <cassert>
#define ALIGN_OF(...) __alignof(__VA_ARGS__)
#define RUMIA_NEW(ALLOCATOR, TYPE, ...) ALLOCATOR.NewObject<TYPE>(__VA_ARGS__)
#define RUMIA_DELETE(ALLOCATOR, TARGET) if(TARGET != nullptr) { ALLOCATOR.DeleteObject(TARGET); TARGET = nullptr; }
#define RUMIA_NEW_ARRAY(ALLOCATOR, TYPE, LENGTH) ALLOCATOR.NewObjectArray<TYPE>(LENGTH)
#define RUMIA_DELETE_ARRAY(ALLOCATOR, TARGET) if(TARGET != nullptr) { ALLOCATOR.DeleteObjectArray(TARGET); TARGET = nullptr; }

namespace Rumia
{
    class Allocator
    {
    public:
        virtual ~Allocator( ) { }

        template <typename T, typename... Args>
        T* NewObject( Args&&... params )
        {
            T* newMemory = ( T* ) ( AllocateAligned( sizeof( T ), ALIGN_OF( T ) ) );
            return new ( newMemory ) T( std::forward<Args>( params )... );
        }

        template <typename T>
        void DeleteObject( T* ptr )
        {
            ptr->~T( );
            DeallocateAligned( ptr, sizeof( T ), ALIGN_OF( T ) );
        }

        template <typename T>
        T* NewObjectArray( size_t length )
        {
            assert( length >= 1 );
            size_t headerSize = sizeof( size_t ) / sizeof( T );

            if ( sizeof( size_t ) % sizeof( T ) > 0 )
            {
                headerSize += 1;
            }
            
            T* origin = ( ( T* ) AllocateAligned( ( (sizeof( T ) * (length + headerSize )) ), ALIGN_OF( T ) ) );
            T* newMemory = origin + headerSize;
            *( ( ( size_t* ) ( newMemory - headerSize ) ) ) = length;

            for ( size_t index = 0; index < length; ++index )
            {
                new ( newMemory + index ) T;
            }

            return newMemory;
        }

        template <typename T>
        void DeleteObjectArray( T* array )
        {
            assert( array != nullptr );
            size_t headerSize = sizeof( size_t ) / sizeof( T );
            if ( sizeof( size_t ) % sizeof( T ) > 0 )
            {
                headerSize += 1;
            }

            size_t length = *( ( ( size_t* ) ( array - headerSize ) ) );
            for ( size_t index = 0; index < length; ++index )
            {
                array[ index ].~T( );
            }
            DeallocateAligned( array - headerSize, ( sizeof( T ) * ( length + headerSize ) ), ALIGN_OF( T ) );
        }

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

        virtual void DeallocateAligned( void* ptr, size_t size, size_t align )
        {
            std::free( ptr );
        }
    };
}