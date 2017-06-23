#pragma once
#include <type_traits>
#include "DefaultAllocator.h"
#include "Iterator.h"
#include "ReverseIterator.h"

namespace Rumia
{
    template <typename T, typename TAllocator = Rumia::DefaultAllocator,
        typename IsChlidOfAllocator = std::enable_if<std::is_base_of<Rumia::Allocator, TAllocator>::value>>
        class List
    {
    protected:
        template <typename Ty>
        struct Node
        {
        public:
            Node( ) :
                m_prev( nullptr ), m_next( nullptr )
            {
            }

            Node( const Ty& data, Node<Ty>* prev, Node<Ty>* next ) :
                m_data( ( data ) ),
                m_prev( prev ), m_next( next )
            {
            }

        public:
            Ty m_data;
            Node<Ty>* m_prev;
            Node<Ty>* m_next;

        };

    public:
        class iterator : public Rumia::Iterator<T, List>
        {
        public:
            iterator( ContainerType& container, Node<T>* currentNode ) :
                Iterator<T, List>( container ), m_currentNode( currentNode )
            {
            }

            ~iterator( ) { }

            iterator& operator++( )
            {
                assert( m_currentNode != nullptr );
                m_currentNode = m_currentNode->m_next;
                return ( *this );
            }

            iterator& operator--( )
            {
                assert( m_currentNode != nullptr );
                m_currentNode = m_currentNode->m_prev;
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
                    ( m_currentNode == rhs.m_currentNode );
            }

            bool operator!=( const iterator& rhs ) const
            {
                return ( &m_container != &rhs.m_container ) ||
                    ( m_currentNode != rhs.m_currentNode );
            }

        private:
            Node<T>* m_currentNode;

        };

        using reverse_iterator = ReverseIterator<T, iterator>;

    public:
        List( ) : List( TAllocator( ) )
        {
        }

        List( TAllocator& allocator ) :
            m_allocator(allocator), 
            m_root( nullptr ),
            m_count( 0 )
        {
        }

        List( const List& target ) :
            List( target.m_allocator )
        {
            Clear( );
            CopyFrom( target.m_root );
        }

        List( List&& target ) :
            List( target.m_allocator )
        {
            Clear( );
            m_root = target.m_root;
            target.m_root = nullptr;

            m_count = target.m_count;
            target.m_count = 0;
        }

        virtual ~List( )
        {
            Clear( );
        }

        List& operator=( const List& rhs )
        {
            // Clear List first
            Clear( );
            m_count = rhs.m_count;

            if ( rhs.m_root != nullptr )
            {
                CopyFrom( rhs.m_root );
            }

            return ( *this );
        }

        List& operator=( List&& rhs )
        {
            assert( &m_allocator == &rhs.m_allocator );
            Clear( );
            m_count = rhs.m_count;
            m_root = rhs.m_root;

            rhs.m_count = 0;
            rhs.m_root = nullptr;

            return ( *this );
        }

        template <typename Ty>
        void PushFront( Ty&& element )
        {
            Node<T>* newNode = RUMIA_NEW( m_allocator, Node<T>, std::forward<Ty>(element), nullptr, nullptr );
            if ( m_root == nullptr )
            {
                m_root = newNode;
            }
            else
            {
                newNode->m_next = m_root;
                m_root->m_prev = newNode;
                m_root = newNode;
            }

            ++m_count;
        }

        template <typename Ty>
        void PushBack( Ty&& element )
        {
            Node<T>* newNode = RUMIA_NEW( m_allocator, Node<T>, std::forward<T>( element ), nullptr, nullptr );
            if ( m_root == nullptr )
            {
                m_root = newNode;
            }
            else
            {
                Node<T>* node = GetMostLastNode( );
                node->m_next = newNode;
                newNode->m_prev = node;
            }

            ++m_count;
        }

        T PopFront( )
        {
            assert( !IsEmpty( ) );
            T data = std::move( m_root->m_data );
            Node<T>* newRoot = m_root->m_next;
            RUMIA_DELETE( m_allocator, m_root );
            m_root = newRoot;

            if ( m_root != nullptr )
            {
                m_root->m_prev = nullptr;
            }

            --m_count;
            return std::move( data );
        }

        T PopBack( )
        {
            assert( !IsEmpty( ) );
            Node<T>* popNode = GetMostLastNode( );
            if ( popNode == m_root )
            {
                m_root = nullptr;
            }

            T data = std::move( popNode->m_data );
            if ( popNode->m_prev != nullptr )
            {
                popNode->m_prev->m_next = nullptr;
            }

            RUMIA_DELETE( m_allocator, popNode );
            --m_count;
            return std::move( data );
        }

        void Clear( )
        {
            for ( Node<T>* node = m_root; node != nullptr; )
            {
                Node<T>* nextNode = node->m_next;
                RUMIA_DELETE( m_allocator, node );
                node = nextNode;
            }

            m_count = 0;
            m_root = nullptr;
        }

        iterator begin( bool bIsDummy = false )
        {
            if ( bIsDummy )
            {
                return iterator( ( *this ), nullptr );
            }

            return iterator( ( *this ), m_root );
        }

        iterator end( bool bIsDummy = true )
        {
            if ( bIsDummy )
            {
                return iterator( ( *this ), nullptr );
            }

            return iterator( ( *this ), GetMostLastNode( ) );
        }

        reverse_iterator rbegin( bool bIsDummy = false )
        {
            return reverse_iterator( end( bIsDummy ) );
        }

        reverse_iterator rend( bool bIsDummy = true )
        {
            return reverse_iterator( begin( bIsDummy ) );
        }

        inline bool IsEmpty( ) const { return ( m_root == nullptr ); }
        inline size_t GetSize( ) const { return m_count; }

    protected:
        void CopyFrom( Node<T>* root )
        {
            assert( ( m_root == nullptr ) );
            Node<T>** targetNode = ( &m_root );
            Node<T>* prevNode = nullptr;
            for ( Node<T>* node = root; node != nullptr; )
            {
                Node<T>* nextNode = node->m_next;
                Node<T>* newNode = RUMIA_NEW( m_allocator, Node<T>, node->m_data, nullptr, nullptr );
                ( *targetNode ) = newNode;
                if ( prevNode != nullptr )
                {
                    prevNode->m_next = newNode;
                    newNode->m_prev = prevNode;
                }
                ( prevNode ) = newNode;
                node = nextNode;
                (targetNode) = &(newNode->m_next);
            }
        }

        inline Node<T>* GetMostLastNode( ) const
        {
            if ( m_root != nullptr )
            {
                Node<T>* node = m_root;
                while ( node->m_next != nullptr )
                {
                    node = node->m_next;
                }

                return node;
            }

            return nullptr;
        }

    protected:
        TAllocator& m_allocator;
        Node<T>* m_root;
        size_t m_count;

    };
}