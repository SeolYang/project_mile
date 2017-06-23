#pragma once
#include <type_traits>
#include "Iterator.h"

namespace Rumia
{
    template <typename Ty, typename ItrTy>
    class ReverseIterator
    {
    public:
        ReverseIterator( const ItrTy& itr ) :
            m_itr( (itr) )
        {
        }

        ReverseIterator& operator++( )
        {
            --m_itr;
            return ( *this );
        }

        ReverseIterator* operator--( )
        {
            ++m_itr;
            return ( *this );
        }

        Ty& operator*( )
        {
            return ( *m_itr );
        }

        Ty& operator*( ) const
        {
            return ( *m_itr );
        }

        bool operator==( const ReverseIterator& rhs ) const
        {
            return ( m_itr == rhs.m_itr );
        }

        bool operator!=( const ReverseIterator& rhs ) const
        {
            return ( m_itr != rhs.m_itr );
        }

    private:
        ItrTy m_itr;

    };
}
