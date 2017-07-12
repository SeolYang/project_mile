#pragma once

namespace Rumia
{
    template <typename ItrTy, typename Ty>
    ItrTy Find( ItrTy begin, ItrTy end, const Ty& data )
    {
        for ( ; begin != end; ++begin )
        {
            if ( ( *begin ) == data )
            {
                return begin;
            }
        }

        return end;
    }

    template <typename ItrTy, typename Ty>
    ItrTy FindNot( ItrTy begin, ItrTy end, const Ty& data )
    {
        for ( ; begin != end; ++begin )
        {
            if ( ( *begin ) != data )
            {
                return begin;
            }
        }

        return end;
    }

    template <typename ItrTy, typename UnaryPredicate>
    ItrTy FindIf( ItrTy begin, ItrTy end, UnaryPredicate pred )
    {
        for ( ; begin != end; ++begin )
        {
            if ( pred( *begin ) )
            {
                return begin;
            }
        }

        return end;
    }

    template <typename ItrTy, typename UnaryPredicate>
    ItrTy FindIfNot( ItrTy begin, ItrTy end, UnaryPredicate pred )
    {
        for ( ; begin != end; ++begin )
        {
            if ( !pred( *begin ) )
            {
                return begin;
            }
        }

        return end;
    }
}