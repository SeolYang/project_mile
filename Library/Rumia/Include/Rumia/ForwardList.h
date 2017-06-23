#pragma once
#include <type_traits>
#include "DefaultAllocator.h"
#include "Iterator.h"

namespace Rumia
{
    template <typename T, typename TAllocator = Rumia::DefaultAllocator,
        typename IsChildOfAllocator = std::enable_if<std::is_base_of<Rumia::Allocator, TAllocator>::value>>
    class ForwardList
    {
    protected:
        template <typename Ty>
        struct Node
        {
        public:
            Node( ) : m_next( nullptr )
            {
            }

            Node( const Ty& data, Node<Ty>* next ) :
                m_data( data ),
                m_next( next )
            {
            }

        public:
            Ty m_data;
            Node<Ty>* m_next;

        };

    public:
        class iterator : public Rumia::Iterator<T, ForwardList>
        {
        public:
            iterator( ContainerType& container, Node<T>* currentNode ) :
                Iterator<T, ForwardList>( container ), m_currentNode( currentNode )
            {
            }

            ~iterator( ) { }

            iterator& operator++( )
            {
                assert( m_currentNode != nullptr );
                m_currentNode = m_currentNode->m_next;
                return ( *this );
            }

            T& operator*( )
            {
                return ( m_currentNode->m_data );
            }

            T& operator*( ) const override
            {
                return ( m_currentNode->m_data );
            }

            bool operator==( const iterator& rhs ) const
            {
                return ( &m_container == &rhs.m_container ) &&
                    ( m_currentNode = rhs.m_currentNode );
            }

            bool operator!=( const iterator& rhs ) const
            {
                return ( &m_container != &rhs.m_container ) ||
                    ( m_currentNode != rhs.m_currentNode );
            }

        private:
            Node<T>* m_currentNode;

        };

    public:
        ForwardList( ) :
            ForwardList( TAllocator( ) )
        {
        }

        ForwardList( TAllocator& allocator ) :
            m_allocator( allocator ),
            m_root( nullptr ),
            m_count( 0 )
        {
        }

        ForwardList( const ForwardList& rhs ) :
            ForwardList( rhs.m_allocator )
        {
            CopyFrom( rhs.m_root );
            m_count = rhs.m_count;
        }

        ForwardList( ForwardList&& rhs ) :
            ForwardList( rhs.m_allocator )
        {
            m_root = rhs.m_root;
            m_count = rhs.m_count;

            rhs.m_root = nullptr;
            rhs.m_count = 0;
        }

        virtual ~ForwardList( )
        {
            Clear( );
        }

        ForwardList& operator=( const ForwardList& rhs )
        {
            // Clear list first
            Clear( );

            CopyFrom( rhs.m_root );
            m_count = rhs.m_count;

            return ( *this );
        }

        ForwardList& operator=( ForwardList&& rhs )
        {
            assert( &m_allocator == &rhs.m_allocator );
            Clear( );

            m_root = rhs.m_root;
            m_count = rhs.m_count;

            rhs.m_root = nullptr;
            rhs.m_count = 0;

            return ( *this );
        }

        template <typename Ty>
        void Push( Ty&& element )
        {
            m_root = RUMIA_NEW( m_allocator, Node<Ty>, std::forward<Ty>(element), m_root );
            ++m_count;
        }

        T Pop( )
        {
            assert( !IsEmpty( ) );
            Node<T>* popedNode = m_root;
            m_root = m_root->m_next;

            T data = popedNode->m_data;
            RUMIA_DELETE( m_allocator, popedNode );
            --m_count;
            return data;
        }

        void Clear( )
        {
            for ( ; m_root != nullptr; )
            {
                Node<T>* nextNode = m_root->m_next;
                m_root->m_data.~T( );
                RUMIA_DELETE( m_allocator, m_root );
                m_root = nextNode;
            }

            m_count = 0;
        }

        iterator begin( )
        {
            return iterator( ( *this ), m_root );
        }

        iterator end( )
        {
            return iterator( ( *this ), nullptr );
        }

        inline bool IsEmpty( ) const { return m_root == nullptr; }
        inline size_t GetSize( ) const { return m_count; }

    protected:
        void CopyFrom( Node<T>* root )
        {
            assert( ( m_root == nullptr ) );
            Node<T>** targetNode = &m_root;
            Node<T>* prevNode = nullptr;
            for ( Node<T>* node = root; node != nullptr; )
            {
                Node<T>* newNode = RUMIA_NEW( m_allocator, Node<T>, node->m_data, nullptr );
                ( *targetNode ) = newNode;
                
                if ( prevNode != nullptr )
                {
                    prevNode->m_next = newNode;
                }
                prevNode = newNode;
                node = root->m_next;
            }
        }

    protected:
        TAllocator& m_allocator;
        Node<T>* m_root;
        size_t m_count;

    };
}