#pragma once

#include <type_traits>
#include "Allocator.h"
#include "Iterator.h"

namespace Rumia
{
    template <typename T>
    class ForwardList
    {
    private:
        struct Node
        {
        public:
            Node( ) : m_next( nullptr )
            {
            }

            Node( const T& data, Node* next ) :
                m_data( data ),
                m_next( next )
            {
            }

        public:
            T m_data;
            Node* m_next;

        };

    public:
        class iterator : public Rumia::Iterator<T, ForwardList>
        {
        public:
            iterator( ContainerType& container, Node* currentNode ) :
                Iterator<T, ForwardList>( container ), m_currentNode( currentNode )
            {
            }

            iterator( const iterator& itr ) :
                iterator( itr.m_container, itr.m_currentNode )
            {
            }

            ~iterator( ) { }

            iterator operator++( )
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
            Node* m_currentNode;

        };

        class const_iterator : public Rumia::Iterator<T, const ForwardList>
        {
        public:
            const_iterator( ContainerType& container, Node* currentNode ) :
                Iterator<T, const ForwardList>( container ), m_currentNode( currentNode )
            {
            }

            ~const_iterator( ) { }

            const_iterator& operator++( )
            {
                assert( m_currentNode != nullptr );
                m_currentNode = m_currentNode->m_next;
                return ( *this );
            }

            T& operator*( ) const override
            {
                return ( m_currentNode->m_data );
            }

            bool operator==( const const_iterator& rhs ) const
            {
                return ( &m_container == &rhs.m_container ) &&
                    ( m_currentNode = rhs.m_currentNode );
            }

            bool operator!=( const const_iterator& rhs ) const
            {
                return ( &m_container != &rhs.m_container ) ||
                    ( m_currentNode != rhs.m_currentNode );
            }

        private:
            Node* m_currentNode;

        };

    public:
        ForwardList( Rumia::Allocator& allocator ) :
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
        void PushFront( Ty&& element )
        {
            m_root = New<Node>( m_allocator, std::forward<Ty>( element ), m_root );
            ++m_count;
        }

        T PopFront( )
        {
            assert( !IsEmpty( ) );
            Node* popedNode = m_root;
            m_root = m_root->m_next;

            T data = popedNode->m_data;
            Delete( m_allocator, popedNode );
            --m_count;
            return data;
        }

        iterator Find( const T& element )
        {
            for ( Node* node = m_root; node != nullptr; node = node->m_next )
            {
                if ( node->m_data == element )
                {
                    return iterator( ( *this ), node );
                }
            }

            return ( this->end( ) );
        }

        const_iterator Find( const T& element ) const
        {
            for ( Node* node = m_root; node != nullptr; node = node->m_next )
            {
                if ( node->m_data == element )
                {
                    return const_iterator( ( *this ), node );
                }
            }

            return ( this->cend( ) );
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

        void Erase( const T& element )
        {
            Node* prevNode = nullptr;
            for ( Node* node = m_root; node != nullptr; node = node->m_next )
            {
                if ( node->m_data == element )
                {
                    if ( prevNode != nullptr )
                    {
                        prevNode->m_next = node->m_next;
                    }

                    if ( node == m_root )
                    {
                        m_root = node->m_next;
                    }

                    Delete( m_allocator, node );
                    --m_count;
                    return;
                }

                prevNode = node;
            }
        }

        iterator Erase( const iterator& itr )
        {
            iterator temp = ( itr );
            ++temp;

            const T& data = ( *itr );
            Erase( data );

            return temp;
        }

        void Clear( )
        {
            for ( ; m_root != nullptr; )
            {
                Node* nextNode = m_root->m_next;
                m_root->m_data.~T( );
                Delete( m_allocator, m_root );
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

        const_iterator cbegin( ) const
        {
            return const_iterator( ( *this ), m_root );
        }

        const_iterator cend( ) const
        {
            return const_iterator( ( *this ), nullptr );
        }

        inline bool IsEmpty( ) const { return m_root == nullptr; }
        inline size_t GetSize( ) const { return m_count; }

    private:
        void CopyFrom( Node* root )
        {
            assert( ( m_root == nullptr ) );
            Node** targetNode = &m_root;
            Node* prevNode = nullptr;
            for ( Node* node = root; node != nullptr; )
            {
                Node* newNode = RUMIA_NEW( m_allocator, Node, node->m_data, nullptr );
                ( *targetNode ) = newNode;
                
                if ( prevNode != nullptr )
                {
                    prevNode->m_next = newNode;
                }
                prevNode = newNode;
                node = root->m_next;
            }
        }

    private:
        Rumia::Allocator& m_allocator;
        Node* m_root;
        size_t m_count;

    };
}