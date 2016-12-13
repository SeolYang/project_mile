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

        MString& operator=( MString&& RHS );
        MString& operator=( const std::wstring& InStr );

        MString operator+( const MString& RHS ) const;
        MString& operator+=( const MString& RHS );

        operator std::wstring( ) const;

        MString SubStr( uint64 Offset, uint64 Count ) const;

        uint64 GetLength( ) const;

        bool IsEmpty( ) const;

    private:
        DataType Data;

    };
}
