#pragma once
#include "MileChar.h"

namespace Mile
{
    class MILE_API MString
    {
        using DataType = std::basic_string<MChar>;

    public:
        MString( );
        MString( const MString& InStr );
        MString( MString&& InStr );
        MString( const DataType& InStr );
        MString( DataType&& InStr );
        MString( const std::wstring& InStr );

        MString& operator=( const MString& RHS );
        MString& operator=( MString&& RHS );
        MString& operator=( const std::wstring& InStr );

        MString operator+( const MString& RHS ) const;
        MString& operator+=( const MString& RHS );

        bool operator==( const MString& RHS ) const;
        bool operator<( const MString& RHS ) const;
        bool operator<=( const MString& RHS ) const;
        bool operator>( const MString& RHS ) const;
        bool operator>=( const MString& RHS ) const;

        inline MChar& operator[]( uint64 Index )
        {
            ASSERT_MSG( Data.size( ) <= Index, "Out of range!" );
            return ( Data[ Index ] );
        }

        operator std::wstring( ) const;

        MString SubStr( uint64 Offset, uint64 Count ) const;

        uint64 GetLength( ) const;

        bool IsEmpty( ) const;

    private:
        DataType Data;

    };
}
