#pragma once

#include "Allocator.h"
#include "Iterator.h"
#include "ReverseIterator.h"
#include "Pair.h"

namespace Rumia
{
    template < typename TKey, typename TValue >
    class Map
    {
    public:
        using TPair = Rumia::Pair<TKey, TValue>;

    private:
        struct Node
        {
        public:
            Node( const TPair& pair, Node* parent = nullptr, Node* left = nullptr, Node* right = nullptr, bool bIsRed = true ) :
                m_pair( pair ),
                m_parent( parent ), m_left( left ), m_right( right ),
                m_bIsRed( bIsRed )
            {
            }

        public:
            TPair m_pair;
            Node* m_parent;
            Node* m_left;
            Node* m_right;
            bool m_bIsRed;

        };

    public:
        Map( Rumia::Allocator& allocator ) :
            m_allocator( allocator ),
            m_root( nullptr )
        {
        }

        TValue& operator[]( const TKey& key )
        {
            return At( key );
        }

        TValue operator[]( const TKey& key ) const
        {
            return At( key );
        }

        void Insert( const TPair& insertValue )
        {
            Node* newNode = RUMIA_NEW( m_allocator, Node, insertValue );
            if ( m_root == nullptr )
            {
                // Root node is black. ( Fix Condition #2 )
                m_root = newNode;
                m_root->m_bIsRed = false;
            }
            else
            {
                // BST Insert
                Node* currentNode = m_root;
                while ( currentNode != nullptr )
                {
                    // Key compare
                    if ( ( currentNode->m_pair ).first > insertValue.first )
                    {
                        if ( currentNode->m_left == nullptr )
                        {
                            currentNode->m_left = newNode;
                            newNode->m_parent = currentNode;
                            currentNode = nullptr;
                        }
                        else
                        {
                            currentNode = currentNode->m_left;
                        }
                    }
                    else if ( ( currentNode->m_pair ).first < insertValue.first )
                    {
                        if ( currentNode->m_right == nullptr )
                        {
                            currentNode->m_right = newNode;
                            newNode->m_parent = currentNode;
                            currentNode = nullptr;
                        }
                        else
                        {
                            currentNode = currentNode->m_right;
                        }
                    }
                    else
                    {
                        // Can't be duplicate
                        assert( false );
                    }
                }

                // New node is red.
                InsertFix( newNode );
            }
        }

        //@Issue#1 Select wrong successor
        void Erase( const TKey& key )
        {
            Node* target = FindNode( key );
            Node* successor = nullptr;
            Node* removeNode = nullptr;

            if ( target != nullptr )
            {
                if ( target->m_left == nullptr || target->m_right == nullptr )
                {
                    removeNode = target;
                }
                else
                {
                    removeNode = InOrderSuccessor( target );
                }

                if ( removeNode->m_left != nullptr )
                {
                    successor = removeNode->m_left;
                }
                else
                {
                    successor = removeNode->m_right;
                }

                if ( successor != nullptr )
                {
                    successor->m_parent = removeNode->m_parent;
                }

                if ( removeNode->m_parent == nullptr )
                {
                    m_root = successor;
                }
                else if ( removeNode == removeNode->m_parent->m_left )
                {
                    removeNode->m_parent->m_left = successor;
                }
                else
                {
                    removeNode->m_parent->m_right = successor;
                }

                if ( target != removeNode )
                {
                    target->m_pair = removeNode->m_pair;
                }

                if ( !removeNode->m_bIsRed )
                {
                    EraseFix( successor );
                }

                RUMIA_DELETE( m_allocator, removeNode );
            }
        }

        TValue& At( const TKey& key )
        {
            Node* foundedNode = FindNode( key );
            return ( *FoundedNode );
        }

        TValue At( const TKey& key ) const
        {
            Node* foundedNode = FindNode( key );
            return ( *foundedNode );
        }

        bool HasKey( const TKey& key ) const { return ( FindNode( key ) != nullptr ); }

    private:
        Node* FindNode( const TKey& key )
        {
            Node* currentNode = m_root;
            while ( currentNode != nullptr )
            {
                if ( ( currentNode->m_pair ).first > key )
                {
                    currentNode = currentNode->m_left;
                }
                else if ( currentNode->m_pair.first < key )
                {
                    currentNode = currentNode->m_right;
                }
                else
                {
                    return currentNode;
                }
            }

            return nullptr;
        }

        void LeftRotate( Node* target )
        {
            if ( target != nullptr )
            {
                Node* parent = target->m_parent;
                Node* replaceNode = target->m_right;
                target->m_right = replaceNode->m_left;
                target->m_parent = replaceNode;

                replaceNode->m_left = target;
                replaceNode->m_parent = parent;

                if ( parent != nullptr )
                {
                    if ( parent->m_right == target )
                    {
                        parent->m_right = replaceNode;
                    }
                    else
                    {
                        parent->m_left = replaceNode;
                    }
                }

                if ( target == m_root )
                {
                    m_root = replaceNode;
                }
            }
        }

        void RightRotate( Node* target )
        {
            if ( target != nullptr )
            {
                Node* parent = target->m_parent;
                Node* replaceNode = target->m_left;
                target->m_left = replaceNode->m_right;
                target->m_parent = replaceNode;

                replaceNode->m_left = target;
                replaceNode->m_parent = parent;

                if ( parent != nullptr )
                {
                    if ( parent->m_right = target )
                    {
                        parent->m_right = replaceNode;
                    }
                    else
                    {
                        parent->m_left = replaceNode;
                    }
                }

                if ( target == m_root )
                {
                    m_root = replaceNode;
                }
            }
        }

        void InsertFix( Node* target )
        {
            Node* me = target;
            while ( me != nullptr && me != m_root && me->m_parent->m_bIsRed )
            {
                Node* parent = me->m_parent;
                Node* grandParent = parent->m_parent;
                Node* uncle = nullptr;

                if ( parent == grandParent->m_left )
                {
                    // ========================== Case 1 Impl
                    uncle = grandParent->m_right;
                    if ( uncle != nullptr && uncle->m_bIsRed )
                    {
                        parent->m_bIsRed = false;
                        uncle->m_bIsRed = false;
                        grandParent->m_bIsRed = true;
                        me = grandParent;
                    }
                    // =========================== Case 2,3 Impl
                    else
                    {
                        if ( me == parent->m_right )
                        {
                            me = parent;
                            LeftRotate( me );

                            parent = me->m_parent;
                            grandParent = parent->m_parent;
                        }

                        parent->m_bIsRed = false;
                        grandParent->m_bIsRed = true;
                        RightRotate( grandParent );
                    }
                }
                else
                {
                    // ========================== Case 4 Impl
                    uncle = grandParent->m_left;
                    if ( uncle != nullptr && uncle->m_bIsRed )
                    {
                        parent->m_bIsRed = false;
                        uncle->m_bIsRed = false;
                        grandParent->m_bIsRed = true;
                        me = grandParent;
                    }
                    // =========================== Case 5, 6 Impl
                    else
                    {
                        if ( me == parent->m_left )
                        {
                            me = parent;
                            RightRotate( me );

                            parent = me->m_parent;
                            grandParent = parent->m_parent;
                        }

                        parent->m_bIsRed = false;
                        grandParent->m_bIsRed = true;
                        LeftRotate( grandParent );
                    }
                }
            }
            m_root->m_bIsRed = false;
        }

        void EraseFix( Node* target )
        {
            Node* me = target;
            if ( me != nullptr )
            {
                while ( me != m_root && !me->m_bIsRed )
                {
                    if ( me == me->m_parent->m_left )
                    {
                        Node* sibling = me->m_parent->m_right;
                        // Case 1
                        if ( sibling->m_bIsRed )
                        {
                            sibling->m_bIsRed = false;
                            me->m_parent->m_bIsRed = true;
                            LeftRotate( me->m_parent );
                        }

                        // Case 2
                        if ( ( !sibling->m_left->m_bIsRed || sibling->m_left == nullptr )
                             && ( !sibling->m_right->m_bIsRed || sibling->m_right == nullptr ) )
                        {
                            sibling->m_bIsRed = true;
                            me = me->m_parent;
                        }
                        // Case 3
                        else if ( !sibling->m_right->m_bIsRed || sibling->m_right == nullptr )
                        {
                            sibling->m_left->m_bIsRed = false;
                            sibling->m_bIsRed = true;
                            RightRotate( sibling );
                            sibling = me->m_parent->m_right;
                        }

                        // Case 4
                        sibling->m_bIsRed = me->m_parent->m_bIsRed;
                        me->m_parent->m_bIsRed = false;
                        sibling->m_right->m_bIsRed = false;
                        LeftRotate( me->m_parent );
                        me = m_root;
                    }
                    else
                    {
                        Node* sibling = me->m_parent->m_left;
                        // Case 5
                        if ( sibling->m_bIsRed )
                        {
                            sibling->m_bIsRed = false;
                            me->m_parent->m_bIsRed = true;
                            RightRotate( me->m_parent );
                        }

                        // Case 6
                        if ( ( !sibling->m_right->m_bIsRed || sibling->m_right == nullptr )
                             && ( !sibling->m_left->m_bIsRed || sibling->m_left == nullptr ) )
                        {
                            sibling->m_bIsRed = true;
                            me = me->m_parent;
                        }
                        // Case 7
                        else if ( !sibling->m_left->m_bIsRed || sibling->m_left == nullptr )
                        {
                            sibling->m_right->m_bIsRed = false;
                            sibling->m_bIsRed = true;
                            LeftRotate( sibling );
                            sibling = me->m_parent->m_left;
                        }

                        // Case 8
                        sibling->m_bIsRed = me->m_parent->m_bIsRed;
                        me->m_parent->m_bIsRed = false;
                        sibling->m_left->m_bIsRed = false;
                        RightRotate( me->m_parent );
                        me = m_root;
                    }
                }

                me->m_bIsRed = false;
            }
        }

        Node* InOrderSuccessor( Node* target )
        {
            if ( target != nullptr )
            {
                if ( target->m_left != nullptr )
                {
                    return MaxValue( target->m_left );
                }
                else
                {
                    Node* me = target;
                    Node* parent = me->m_parent;
                    while ( parent != nullptr && me == parent->m_left )
                    {
                        me = parent;
                        parent = parent->m_parent;
                    }

                    return parent;
                }
            }
        }

        Node* MinValue( Node* target )
        {
            Node* temp = target;
            while ( temp != nullptr && temp->m_left != nullptr )
            {
                temp = temp->m_left;
            }
            return temp;
        }

        Node* MaxValue( Node* target )
        {
            Node* temp = target;
            while ( temp != nullptr && temp->m_right != nullptr )
            {
                temp = temp->m_right;
            }
            return temp;
        }

    private:
        Rumia::Allocator&      m_allocator;

        public:
        Node*                  m_root;

    };
}