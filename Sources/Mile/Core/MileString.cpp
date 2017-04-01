#include "MileString.h"

namespace Mile
{
    MString::MString( )
    {
    }

    MString::MString( const MString& InStr ) :
        Data( InStr.Data )
    {
    }

    MString::MString( MString&& InStr ) :
        Data( std::move( InStr.Data ) )
    {
    }

    MString::MString( const DataType& InStr ) :
        Data( InStr )
    {
    }

    MString::MString( DataType&& InStr ) :
        Data( std::move( InStr ) )
    {
    }

    MString::MString( const std::wstring& InStr )
    {
        ( *this ) = InStr;
    }

    MString::MString( const wchar_t* InStr ) :
        MString( std::wstring( InStr ) )
    {
    }

    MString& MString::operator=( const MString& RHS )
    {
        Data = RHS.Data;
        return ( *this );
    }

    MString& MString::operator=( MString&& RHS )
    {
        Data = std::move( RHS.Data );
        return ( *this );
    }

    MString& MString::operator=( const std::wstring& InStr )
    {
        Data.clear( );

        size_t Size = InStr.size( );
        Data.resize( Size );
        for ( int Index = 0; Index < Size; ++Index )
        {
            Data[ Index ] = InStr[ Index ];
        }

        return ( *this );
    }

    MString& MString::operator=( const wchar_t* InStr )
    {
        return ( ( *this ) = std::wstring( InStr ) );
    }

    MString MString::operator+( const MString& RHS ) const
    {
        return MString( Data + RHS.Data );
    }

    MString& MString::operator+=( const MString& RHS )
    {
        Data += RHS.Data;
        return ( *this );
    }

    bool MString::operator==( const MString& RHS ) const
    {
        return ( Data == RHS.Data );
    }

    bool MString::operator<( const MString& RHS ) const
    {
        return ( Data < RHS.Data );
    }

    bool MString::operator<=( const MString& RHS ) const
    {
        return ( Data <= RHS.Data );
    }

    bool MString::operator>( const MString& RHS ) const
    {
        return ( Data > RHS.Data );
    }

    bool MString::operator>=( const MString& RHS ) const
    {
        return ( Data >= RHS.Data );
    }

    MString::operator std::wstring( ) const
    {
        std::wstring OutStr;

        size_t Size = Data.size( );
        OutStr.resize( Size );
        for ( int Index = 0; Index < Size; ++Index )
        {
            OutStr[ Index ] = Data[ Index ];
        }

        return OutStr;
    }

    MString MString::SubStr( uint64 Offset, uint64 Count = DataType::npos ) const
    {
        return MString( Data.substr( Offset, Count ) );
    }

    uint64 MString::GetLength( ) const
    {
        return Data.length( );
    }

    bool MString::IsEmpty( ) const
    {
        return Data.empty( );
    }
}