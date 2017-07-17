#pragma once

#include <type_traits>
#include "Allocator.h"
#include "Iterator.h"
#include "ReverseIterator.h"

namespace Rumia
{
    /* Rumia::Array is Dynamic Array class **/
    template <typename T>
        class Array
    {
    public:
        class iterator : public Rumia::Iterator<T, Array>
        {
        public:
            friend Array;
            iterator( ContainerType& container, size_t index, bool bIsDummy = false ) :
                Rumia::Iterator<T, Array>( container ), m_position( index ), m_bIsDummy( bIsDummy )
            {
            }

            iterator( const iterator& itr ) :
                iterator( itr.m_container, itr.m_position, itr.m_bIsDummy )
            {
            }

            ~iterator( ) { }

            iterator operator++( )
            {
                if ( m_position >= ( m_container.GetSize( ) - 1 ) && !m_bIsDummy )
                {
                    m_position = m_container.GetSize( ) - 1;
                    m_bIsDummy = true;
                }
                else
                {
                    ++m_position;
                    m_bIsDummy = false;
                }
                return ( *this );
            }

            iterator operator--( )
            {
                if ( m_position == 0 && !m_bIsDummy )
                {
                    m_bIsDummy = true;
                }
                else
                {
                    --m_position;
                    m_bIsDummy = false;
                }

                return ( *this );
            }

            virtual T& operator*( )
            {
                return m_container[ m_position ];
            }

            virtual T& operator*( ) const override
            {
                return m_container[ m_position ];
            }

            bool operator==( const iterator& rhs ) const
            {
                return ( &m_container == &rhs.m_container ) &&
                    ( m_position == rhs.m_position ) &&
                    ( m_bIsDummy == rhs.m_bIsDummy );
            }

            bool operator!=( const iterator& rhs ) const
            {
                return ( &m_container != &rhs.m_container ) ||
                    ( m_position != rhs.m_position ) ||
                    ( m_bIsDummy != rhs.m_bIsDummy );
            }

            bool operator>( const iterator& rhs ) const
            {
                return ( &m_container == &rhs.m_container ) &&
                    ( m_position > rhs.m_position );
            }

            bool operator<( const iterator& rhs ) const
            {
                return ( &m_container == &rhs.m_container ) &&
                    ( m_position < rhs.m_position );
            }

            bool operator>=( const iterator& rhs ) const
            {
                return ( &m_container == &rhs.m_container ) &&
                    ( m_position >= rhs.m_position );
            }

            bool operator<=( const iterator& rhs ) const
            {
                return ( &m_container == &rhs.m_container ) &&
                    ( m_position <= rhs.m_position );
            }

        private:
            size_t m_position;

            bool m_bIsDummy;

        };

        class const_iterator : public Rumia::Iterator<T, const Array>
        {
        public:
            friend Array;
            const_iterator( ContainerType& container, size_t index, bool bIsDummy = false ) :
                Rumia::Iterator<T, const Array>( container ), m_position( index ), m_bIsDummy( bIsDummy )
            {
            }

            ~const_iterator( ) { }

            const_iterator& operator++( )
            {
                if ( m_position == ( m_container.GetSize( ) - 1 ) && !m_bIsDummy )
                {
                    m_bIsDummy = true;
                }
                else
                {
                    ++m_position;
                    m_bIsDummy = false;
                }
                return ( *this );
            }

            const_iterator& operator--( )
            {
                if ( m_position == 0 && !m_bIsDummy )
                {
                    m_bIsDummy = true;
                }
                else
                {
                    --m_position;
                    m_bIsDummy = false;
                }

                return ( *this );
            }

            virtual T& operator*( ) const override
            {
                return m_container[ m_position ];
            }

            bool operator==( const const_iterator& rhs ) const
            {
                return ( &m_container == &rhs.m_container ) &&
                    ( m_position == rhs.m_position ) &&
                    ( m_bIsDummy == rhs.m_bIsDummy );
            }

            bool operator!=( const const_iterator& rhs ) const
            {
                return ( &m_container != &rhs.m_container ) ||
                    ( m_position != rhs.m_position ) ||
                    ( m_bIsDummy != rhs.m_bIsDummy );
            }

            bool operator>( const const_iterator& rhs ) const
            {
                return ( &m_container == &rhs.m_container ) &&
                    ( m_position > rhs.m_position );
            }

            bool operator<( const const_iterator& rhs ) const
            {
                return ( &m_container == &rhs.m_container ) &&
                    ( m_position < rhs.m_position );
            }

            bool operator>=( const const_iterator& rhs ) const
            {
                return ( &m_container == &rhs.m_container ) &&
                    ( m_position >= rhs.m_position );
            }

            bool operator<=( const const_iterator& rhs ) const
            {
                return ( &m_container == &rhs.m_container ) &&
                    ( m_position <= rhs.m_position );
            }

        private:
            size_t m_position;

            bool m_bIsDummy;
        };

        using reverse_iterator = ReverseIterator<T, iterator>;

    public:
        Array( Rumia::Allocator& allocator, size_t initCapacity = 2 ) :
            m_allocator( allocator ),
            m_elements( nullptr ), m_capacity( 0 ), m_size( 0 )
        {
            Reserve( initCapacity );
        }

        Array( const Array& target ) :
            m_allocator( target.m_allocator ), m_elements( nullptr ), m_capacity( 0 ), m_size( 0 )
        {
            // Reserve Memory space
            Reserve( target.m_capacity );

            // Copy elements
            for ( size_t index = 0; index < target.m_size; ++index )
            {
                m_elements[ index ] = target.m_elements[ index ];
            }
            m_size = target.m_size;
        }

        Array( Array&& target ) :
            m_allocator( target.m_allocator ),
            m_elements( target.m_elements ),
            m_capacity( target.m_capacity ),
            m_size( target.m_size )
        {
            target.m_elements = nullptr;
            target.m_capacity = 0;
            target.m_size = 0;
        }

        virtual ~Array( )
        {
            DeleteArray( m_allocator, m_elements );
        }

        T& operator[]( size_t index )
        {
            /* Out of range **/
            assert( !IsOutOfRange( index ) );
            return ( m_elements[ index ] );
        }

        // const index operator
        T& operator[]( size_t index ) const
        {
            /* Out of range **/
            assert( !IsOutOfRange( index ) );
            return ( m_elements[ index ] );
        }

        // copy operator
        Array& operator=( const Array& target )
        {
            if ( this != &target )
            {
                // Clear First
                Clear( );

                // Reserve Memory space
                Reserve( target.m_capacity );

                // Copy elements
                for ( size_t index = 0; index < target.m_size; ++index )
                {
                    m_elements[ index ] = target.m_elements[ index ];
                }
                m_size = target.m_size;
            }

            return ( *this );
        }

        // move operator
        Array& operator=( Array&& target )
        {
            if ( this != &target )
            {
                assert( &m_allocator == &target.m_allocator );
                Clear( );

                // Move element array
                m_elements = target.m_elements;
                m_capacity = target.m_capacity;
                m_size = target.m_size;

                target.m_elements = nullptr;
                target.m_capacity = 0;
                target.m_size = 0;
            }

            return ( *this );
        }

        template <typename Ty>
        void PushBack( Ty&& element )
        {
            Growth( );
            m_elements[ m_size++ ] = std::forward<Ty>( element );
        }

        T PopBack( )
        {
            assert( !IsEmpty( ) );
            return m_elements[ --m_size ];
        }

        template <typename... Args>
        void Emplace( Args&&... params )
        {
            Growth( );
            m_elements[ m_size++ ] = T( std::forward<Args>( params )... );
        }

        template < typename Ty >
        void Insert( size_t positionIndex, Ty&& instance )
        {
            assert( !IsOutOfRange( positionIndex ) || ( positionIndex == m_size ) );
            Growth( );
            for ( size_t index = m_size - 1; index > positionIndex; --index )
            {
                m_elements[ index + 1 ] = std::move( m_elements[ index ] );
            }

            m_elements[ positionIndex ] = std::forward<Ty>( instance );
            ++m_size;
        }

        void Resize( size_t targetSize )
        {
            if ( targetSize <= m_size )
            {
                for ( size_t index = ( m_size - 1 ); index > targetSize; --index )
                {
                    m_elements[ index ].~T( );
                }
            }
            else if ( targetSize > m_capacity )
            {
                Reserve( targetSize );
            }

            m_size = targetSize;
        }

        void Resize( size_t targetSize, const T& placeHolder )
        {
            size_t originSize = m_size;
            Resize( targetSize );

            if ( originSize < m_size )
            {
                for ( size_t index = originSize; index < m_size; ++index )
                {
                    m_elements[ index ] = placeHolder;
                }
            }
        }

        void Reserve( size_t targetCapacity )
        {
            assert( targetCapacity != 0 && targetCapacity > m_capacity );
            if ( targetCapacity != m_capacity )
            {
                T* cachedElements = m_elements;
                m_elements = m_allocator.NewObjectArray<T>( targetCapacity );
                for ( size_t index = 0; index < m_size; ++index )
                {
                    m_elements[ index ] = std::move( cachedElements[ index ] );
                }

                DeleteArray( m_allocator, cachedElements );

                m_capacity = targetCapacity;
            }
        }

        void Shrink( )
        {
            if ( HasUnusedCapacity( ) )
            {
                T* tempElements = m_elements;
                m_elements = NewArray<T>( m_allocator, m_size );
                for ( size_t Idx = 0; Idx < m_size; ++Idx )
                {
                    m_elements[ Idx ] = tempElements[ Idx ];
                }

                m_capacity = m_size;
                DeleteArray( m_allocator, tempElements );
            }
        }

        T& At( size_t index )
        {
            return m_elements[ index ];
        }

        T& At( size_t index ) const
        {
            return m_elements[ index ];
        }

        iterator Find( const T& target )
        {
            // If it didn't found target. It'll be return dummy iterator
            size_t foundIdx = IndexOf( target );
            return iterator( ( *this ), foundIdx, ( foundIdx == m_size ) );
        }

        template <typename UnaryPredicate>
        iterator FindIf( UnaryPredicate pred )
        {
            for ( auto itr = begin( ); itr != end( ); ++itr )
            {
                if ( pred( *itr ) )
                {
                    return itr;
                }
            }

            return end( );
        }

        const_iterator Find( const T& target ) const
        {
            size_t foundIdx = IndexOf( target );
            return const_iterator( ( *this ), foundIdx, ( foundIdx == m_size ) );
        }

        template <typename UnaryPredicate>
        const_iterator FindIf( UnaryPredicate pred ) const
        {
            for ( auto itr = cbegin( ); itr != cend( ); ++itr )
            {
                if ( pred( *itr ) )
                {
                    return itr;
                }
            }

            return cend( );
        }

        // When method didn't find out passed element, it'll be return size of array.
        size_t IndexOf( const T& target ) const
        {
            for ( size_t index = 0; index < m_size; ++index )
            {
                if ( m_elements[ index ] == target )
                {
                    return index;
                }
            }

            return m_size;
        }

        void Erase( const T& element )
        {
            size_t foundIndex = IndexOf( element );
            if ( foundIndex != m_size )
            {
                EraseAt( foundIndex );
            }
        }

        iterator Erase( const iterator& itr )
        {
            EraseAt( itr.m_position );
            return itr;
        }

        void EraseAt( size_t index )
        {
            assert( !IsOutOfRange( index ) );
            m_elements[ index ].~T( );
            --m_size;

            for ( size_t itrIndex = index; itrIndex < m_size; ++itrIndex )
            {
                m_elements[ itrIndex ] = std::move( m_elements[ itrIndex + 1 ] );
            }
        }

        void Clear( bool resetReserve = true )
        {
            if ( resetReserve )
            {
                m_capacity = 0;
                m_size = 0;
                DeleteArray( m_allocator, m_elements );
            }
            else
            {
                m_size = 0;
            }
        }

        iterator begin( bool bIsDummy = false )
        {
            return iterator( ( *this ), 0, bIsDummy );
        }

        iterator end( bool bIsDummy = true )
        {
            return iterator( ( *this ), m_size - 1, bIsDummy );
        }

        const_iterator cbegin( bool bIsDummy = false ) const
        {
            return const_iterator( ( *this ), 0, bIsDummy );
        }

        const_iterator cend( bool bIsDummy = true ) const
        {
            return const_iterator( ( *this ), m_size - 1, bIsDummy );
        }

        reverse_iterator rbegin( bool bIsDummy = false )
        {
            return reverse_iterator( end( bIsDummy ) );
        }

        reverse_iterator rend( bool bIsDummy = true )
        {
            return reverse_iterator( begin( bIsDummy ) );
        }

        inline size_t GetCapacity( ) const { return m_capacity; }
        inline size_t GetSize( ) const { return m_size; }
        inline bool IsEmpty( ) const { return ( m_size == 0 ); }
        inline bool IsFull( ) const { return ( m_size == m_capacity ); }

    private:
        inline void Growth( )
        {
            if ( IsFull( ) )
            {
                if ( m_capacity > 0 )
                {
                    Reserve( m_capacity * CapacityExtendScale );
                }
                else
                {
                    Reserve( 2 );
                }
            }
        }

        inline bool IsOutOfRange( size_t index ) const
        {
            return ( index >= m_size );
        }

        inline bool HasUnusedCapacity( ) const
        {
            return ( ( m_capacity - m_size ) > 0 );
        }

    private:
        Rumia::Allocator& m_allocator;

        /** Size of Element buffer */
        size_t m_capacity;

        /* Number of Elements **/
        size_t m_size;

        T* m_elements;

        static const unsigned int CapacityExtendScale = 2;

    };
}