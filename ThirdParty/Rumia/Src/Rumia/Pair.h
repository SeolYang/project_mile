#pragma once

#include "Tuple.h"

namespace Rumia
{
    template< typename FirstTy, typename SecondTy >
    class Pair
    {
    public:
        using FirstType = FirstTy;
        using SecondType = SecondTy;

    public:
        Pair( ) :
            first( FirstTy( ) ),
            second( SecondTy( ) )
        {
        }

        template <typename _FirstTy, typename _SecondTy>
        Pair( _FirstTy&& firstData, _SecondTy&& secondData ) :
            first( std::forward<_FirstTy>( firstData ) ),
            second( std::forward<_SecondTy>( secondData ) )
        {

        }

    public:
        FirstTy first;
        SecondTy second;

    };
}

namespace Rumia
{
    template <typename FirstTy, typename SecondTy>
    Rumia::Pair<FirstTy, SecondTy> MakePair( FirstTy&& first, SecondTy&& second )
    {
        return Rumia::Pair<FirstTy, SecondTy>( std::forward<FirstTy>( first ),
                                               std::forward<SecondTy>( second ) );
    }

    namespace _Impl
    {
        template <size_t index, typename FirstTy, typename SecondTy>
        struct PairGetImpl
        {
        public:
            static decltype( auto ) value( const Pair<FirstTy, SecondTy>& pair );
        };

        template < typename FirstTy, typename SecondTy>
        struct PairGetImpl<0, FirstTy, SecondTy>
        {
        public:
            static decltype( auto ) value( const Pair<FirstTy, SecondTy>& pair )
            {
                return pair.first;
            }
        };

        template < typename FirstTy, typename SecondTy>
        struct PairGetImpl<1, FirstTy, SecondTy>
        {
        public:
            static decltype( auto ) value( const Pair<FirstTy, SecondTy>& pair )
            {
                return pair.second;
            }
        };
    }

    template <size_t index, typename FirstTy, typename SecondTy>
    decltype( auto ) Get( const Pair<FirstTy, SecondTy>& pair )
    {
        return _Impl::PairGetImpl<index, FirstTy, SecondTy>::value( pair );
    }

    template<typename FirstTy, typename SecondTy>
    struct TupleSize<Pair<FirstTy, SecondTy>>
    {
        static const size_t value = 2;
    };
}