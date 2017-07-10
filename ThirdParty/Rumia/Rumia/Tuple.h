#pragma once

namespace Rumia
{
    template <typename Ty, typename... Rest>
    class Tuple : public Tuple<Rest...>
    {
    public:
        template <typename _Ty>
        Tuple( _Ty&& data, Rest... rest ) : 
            Tuple<Rest...>( rest... ), 
            m_data( std::forward<_Ty>( data ) )
        {
        }

    public:
        Ty m_data;

    };

    template <typename Ty>
    class Tuple<Ty>
    {
    public:
        template <typename _Ty>
        Tuple( _Ty&& data ) :
            m_data( std::forward<_Ty>( data ) )
        {
        }

    public:
        Ty m_data;

    };
}

namespace Rumia
{
    template <typename Ty, typename... Rest>
    Rumia::Tuple<Ty, Rest...> MakeTuple( Ty&& data, Rest... rest )
    {
        return Rumia::Tuple<Ty, Rest...>( std::forward<Ty>( data ), rest... );
    }

    namespace _Impl
    {
        template <size_t index, typename Ty, typename... Rest>
        struct GetImpl
        {
        public:
            static decltype( auto ) value( const Tuple<Ty, Rest...>& tuple )
            {
                return GetImpl< index - 1, Rest...>::value( tuple );
            }
        };

        template <typename Ty, typename... Rest>
        struct GetImpl<0, Ty, Rest...>
        {
        public:
            static Ty value( const Tuple<Ty, Rest...>& tuple )
            {
                return tuple.m_data;
            }
        };
    }

    template <size_t index, typename Ty, typename... Rest>
    decltype( auto ) Get( const Tuple<Ty, Rest...>& tuple )
    {
        return _Impl::GetImpl<index, Ty, Rest...>::value( tuple );
    }

    template <typename Tuple>
    struct TupleSize;

    template <typename... Types>
    struct TupleSize<Tuple<Types...>>
    {
        const static size_t value = sizeof...( Types );
    };
}