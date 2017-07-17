#pragma once

#include <cstdlib>
#include <cassert>
#include <memory>
#include <functional>

#define ALIGN_OF(...) __alignof(__VA_ARGS__)

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
            memset( newMemory, 0, sizeof( T ) * length );

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

    template < typename Ty, typename... Args>
    static Ty* New( Allocator& allocator, Args&&... params )
    {
        return allocator.NewObject<Ty>( std::forward<Args>( params )... );
    }

    template < typename Ty >
    static void Delete( Allocator& allocator, Ty& target )
    {
        if ( target != nullptr )
        {
            allocator.DeleteObject( target );
            target = nullptr;
        }
    }

    template < typename Ty >
    static Ty* NewArray( Allocator& allocator, size_t length )
    {
        if ( length > 0 )
        {
            return allocator.NewObjectArray<Ty>( length );
        }

        return nullptr;
    }

    template < typename Ty >
    static void DeleteArray( Allocator& allocator, Ty& target )
    {
        if ( target != nullptr )
        {
            allocator.DeleteObjectArray( target );
            target = nullptr;
        }
    }

    template < typename Ty, typename... Args >
    static std::shared_ptr<Ty> MakeShared( Allocator& allocator, Args&&... params )
    {
        return std::shared_ptr<Ty>(
            allocator.NewObject<Ty>( std::forward<Args>( params )... ),
            std::bind( &Rumia::Allocator::DeleteObject<Ty>, &allocator, std::placeholders::_1 ) );
    }

    template < typename Ty, typename... Args >
    static std::unique_ptr<Ty, std::function<void( Ty* )>> MakeUnique( Allocator& allocator, Args&&... params )
    {
        return std::unique_ptr<Ty, std::function<void( Ty* )>>(
            allocator.NewObject<Ty>( std::forward<Args>( params )... ),
            std::bind( &Rumia::Allocator::DeleteObject<Ty>, &allocator, std::placeholders::_1 ) );
    }
}