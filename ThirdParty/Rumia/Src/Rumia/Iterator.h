#pragma once

namespace Rumia
{
    template<typename Ty, typename ContainerTy>
    class Iterator
    {
    public:
        using DataType = Ty;
        using ContainerType = ContainerTy;

    public:
        Iterator( ContainerType& container ) : m_container( container )
        {
        }

        virtual ~Iterator( ) { }

        virtual Ty& operator*( ) const = 0;

    protected:
        ContainerType& m_container;

    };
}