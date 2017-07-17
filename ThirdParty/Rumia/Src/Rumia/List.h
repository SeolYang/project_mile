#pragma once

#include <type_traits>
#include "Allocator.h"
#include "Iterator.h"
#include "ReverseIterator.h"

namespace Rumia
{
    template <typename T>
        class List
    {
    private:
        struct Node
        {
        public:
            Node( ) :
                m_prev( nullptr ), m_next( nullptr )
            {
            }

            Node( const T& data, Node* prev, Node* next ) :
                m_data( ( data ) ),
                m_prev( prev ), m_next( next )
            {
            }

        public:
            T m_data;
            Node* m_prev;
            Node* m_next;

        };

    public:
        class iterator : public Rumia::Iterator<T, List>
        {
        public:
            iterator( ContainerType& container, Node* currentNode ) :
                Iterator<T, List>( container ), m_currentNode( currentNode )
            {
            }

            iterator( const iterator& itr ) :
                iterator( itr.m_container, itr.m_currentNode )
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
            Node* m_currentNode;

        };

        class const_iterator : public Rumia::Iterator<T, const List>
        {
        public:
            const_iterator( ContainerType& container, Node* currentNode ) :
                Iterator<T, const List>( container ), m_currentNode( currentNode )
            {
            }

            ~const_iterator( ) { }

            const_iterator& operator++( )
            {
                assert( m_currentNode != nullptr );
                m_currentNode = m_currentNode->m_next;
                return ( *this );
            }

            const_iterator& operator--( )
            {
                assert( m_currentNode != nullptr );
                m_currentNode = m_currentNode->m_prev;
                return ( *this );
            }

            T& operator*( ) const override
            {
                return ( m_currentNode->m_data );
            }

            bool operator==( const const_iterator& rhs ) const
            {
                return ( &m_container == &rhs.m_container ) &&
                    ( m_currentNode == rhs.m_currentNode );
            }

            bool operator!=( const const_iterator& rhs ) const
            {
                return ( &m_container != &rhs.m_container ) ||
                    ( m_currentNode != rhs.m_currentNode );
            }

        private:
            Node* m_currentNode;

        };

        using reverse_iterator = ReverseIterator<T, iterator>;

    public:
        List( Rumia::Allocator& allocator ) :
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
            Node* newNode = New<Node>( m_allocator, std::forward<Ty>( element ), nullptr, nullptr );
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
            Node* newNode = New<Node>( m_allocator, std::forward<T>( element ), nullptr, nullptr );
            if ( m_root == nullptr )
            {
                m_root = newNode;
            }
            else
            {
                Node* node = GetMostLastNode( );
                node->m_next = newNode;
                newNode->m_prev = node;
            }

            ++m_count;
        }

        T PopFront( )
        {
            assert( !IsEmpty( ) );
            T data = std::move( m_root->m_data );
            Node* newRoot = m_root->m_next;
            Delete( m_allocator, m_root );
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
            Node* popNode = GetMostLastNode( );
            if ( popNode == m_root )
            {
                m_root = nullptr;
            }

            T data = std::move( popNode->m_data );
            if ( popNode->m_prev != nullptr )
            {
                popNode->m_prev->m_next = nullptr;
            }

            Delete( m_allocator, popNode );
            --m_count;
            return std::move( data );
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
            for ( Node* node = m_root; node != nullptr;)
            {
                if ( element == ( node->m_data ) )
                {
                    --m_count;
                    if ( node->m_prev != nullptr )
                    {
                        node->m_prev->m_next = node->m_next;
                    }

                    if ( node->m_next != nullptr )
                    {
                        node->m_next->m_prev = node->m_prev;
                    }

                    if ( node == m_root )
                    {
                        m_root = nullptr;
                        m_root = node->m_next;
                    }

                    Delete( m_allocator, node );
                    return;
                }

                node = node->m_next;
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
            for ( Node* node = m_root; node != nullptr; )
            {
                Node* nextNode = node->m_next;
                Delete( m_allocator, node );
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

        const_iterator cbegin( bool bIsDummy = false ) const
        {
            if ( bIsDummy )
            {
                return const_iterator( ( *this ), nullptr );
            }

            return const_iterator( ( *this ), m_root );
        }

        const_iterator cend( bool bIsDummy = true ) const
        {
            if ( bIsDummy )
            {
                return const_iterator( ( *this ), nullptr );
            }

            return const_iterator( ( *this ), GetMostLastNode( ) );
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

    private:
        void CopyFrom( Node* root )
        {
            assert( ( m_root == nullptr ) );
            Node** targetNode = ( &m_root );
            Node* prevNode = nullptr;
            for ( Node* node = root; node != nullptr; )
            {
                Node* nextNode = node->m_next;
                Node* newNode = New<Node>( m_allocator, node->m_data, nullptr, nullptr );
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

        inline Node* GetMostLastNode( ) const
        {
            if ( m_root != nullptr )
            {
                Node* node = m_root;
                while ( node->m_next != nullptr )
                {
                    node = node->m_next;
                }

                return node;
            }

            return nullptr;
        }

    private:
        Rumia::Allocator& m_allocator;
        Node* m_root;
        size_t m_count;

    };
}
