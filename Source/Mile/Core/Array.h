#pragma once
#include "Mile.h"
#include "Allocator.h"

/**
* 동적 크기를가지는 배열 클래스
*/
template < class T >
class MArray final
{
public:
    MArray( ) = delete;
    explicit MArray( MAllocator& Source ) :
        Allocator( Source ),
        Data( nullptr ), Capacity( 0 ), Size( 0 )
    {

    }

    explicit MArray( const MArray& Source ) :
        Allocator( Source.Allocator ),
        Data( nullptr ), Capacity( 0 ), Size( 0 )
    {
        ( *this ) = Source;
    }

    MArray& operator=( const MArray& RHS )
    {
        if ( Capacity != 0 )
        {
            Allocator.Deallocate( Data );
            Data = nullptr;
        }

        Allocator = RHS.Allocator;
        Capactiy = RHS.Capacity;
        Size = RHS.Size;
        Reserve( Capacity );
        memcpy(
            Data,
            RHS.Data,
            sizeof( T ) * Size
        );

        return ( *this );
    }

    MArray& operator=( MArray&& RHS )
    {
        if ( Capacity != 0 )
        {
            Allocator.Deallocate( Data );
            Data = nullptr;
        }

        Allocator = RHS.Allocator;
        Capacity = RHS.Capacity;
        Size = RHS.Size;
        Data = RHS.Data;

        RHS.Capacity = 0;
        RHS.Size = 0;
        RHS.Data = nullptr;

        return ( *this );
    }

    T& operator[]( uint64 Index )
    {
        ASSERT_MSG( Index < Size, "Out of range!" );
        return ( Data[Index] );
    }

    void Push( const T& Element )
    {
        if ( Size == Capacity )
        {
            Expand( );
        }

        auto Address = ( Data + Size );
        new ( Address ) T( Element ); /* Call Constructor **/
        ++Size;
    }

    void Pop( )
    {
        if ( Size > 0 )
        {
            Data[( Size - 1 )].~T( );
            --Size;
        }
    }

    template <typename... ParamsT>
    T& Emplace( ParamsT&&... Params )
    {
        if ( Size == Capacity )
        {
            Expand( );
        }

        auto Address = ( Data + Size );
        new ( Address ) T( std::forward<ParamsT>( Params )... );
        ++Size;

        return Data[Size - 1];
    }

    void Insert( uint64 Index, const T& Element )
    {
        ASSERT_MSG( Index < Size, "Out of range!" )
            if ( Size == Capacity )
            {
                Expand( );
            }

        memmove(
            Data + ( Index + 1 ),				/** 옮길 메모리 주소 */
            Data + Index,						/** 원래 메모리 주소 */
            sizeof( T ) * ( Size - Index )
        );	/** 옮길 메모리 크기 */

        Data[Index] = Element;
        ++Size;
    }

    inline void Reserve( uint64 NewCapacity )
    {
        if ( NewCapacity > Capacity )
        {
            T* NewAddress = (T*)Allocator.Allocate( NewCapacity * sizeof( T ) );

            memcpy( NewAddress, Data, sizeof( T ) * Size );

            Allocator.Deallocate( Data );

            Data = NewAddress;
            Capacity = NewCapacity;
        }
    }

    inline void Erase( uint64 Index )
    {
        ASSERT_MSG( Index < Size, "Out of range!" );
        ( Data + Index )->~T( );

        memmove(
            ( Data + Index ),
            ( Data + Index + 1 ),
            ( Size - Index );
        --Size;
    }

    void Erase( const T& Element )
    {
        for ( uint64 Index = 0; Index < Size; ++Index )
        {
            if ( Element == Data[Index] )
            {
                Erase( Index );
                break;
            }
        }
    }

    uint64 GetCapacity( ) const
    {
        return Capacity;
    }

    uint64 GetSize( ) const
    {
        return Size;
    }

public:
    static void Swap( MArray& Target1, MArray& Target2 )
    {
        MAllocator& TempAllocator = Target2.Allocator;
        T* TempDataAddress = Target2.Data;
        uint64 TempCapacity = Target2.Capacity;
        uint64 TempSize = Target2.Size;

        Target2.Allocator = Target1.Allocator;
        Target2.Data = Target1.Data;
        Target2.Capacity = Target1.Capacity;
        Target2.Size = Target1.Size;

        Target1.Allocator = TempAllocator;
        Target1.Data = TempDataAddress;
        Target1.Capacity = TempCapacity;
        Target2.Size = TempSize;
    }

private:
    void Expand( )
    {
        uint64 TargetCapacity = ( Size == 0 ) ? InitialCapacity : ( Size * ExpandScale );
        Reserve( TargetCapacity );
    }

private:
    MAllocator&		Allocator;
    T*				Data;

    /**
    * 배열의 길이(크기)
    */
    uint64			Capacity;

    /**
    * 실제로 배열안에 들어있는 원소의 갯수
    */
    uint64			Size;

    constexpr uint8 InitialCapacity = 4;
    constexpr uint8 ExpandScale = 2;

};