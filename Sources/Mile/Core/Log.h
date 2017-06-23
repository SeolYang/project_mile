#pragma once
#include "MileManager.hpp"
#include "MileTimer.h"

namespace Mile
{
    enum class MILE_API LogType
    {
        Log,
        Debug,
        Warning,
        Error,
        Critical
    };

    struct MILE_API Log
    {
    public:
        Log( LogType Type, const MString& Message, const MString& Time ) :
            Type( Type ), Message( Message ), Time( Time )
        {
        }

        MString ToString( ) const
        {
            return ( MString ) ( *this );
        }

        operator MString( ) const;

    public:
        LogType Type;
        MString Message;
        MString Time;

    };

    class MILE_API Logger : public Mile::Manager<Logger>
    {
        friend Manager;

    public:
        Logger( const Logger& ) = delete;
        Logger& operator=( const Logger& ) = delete;

        void Log( LogType Type, const MString& Message );
        void Log( const MString& Message );
        void Debug( const MString& Message );
        void Warning( const MString& Message );
        void Error( const MString& Message );
        void Critical( const MString& Message );

        void Clear( );
        void Flush( );

    private:
        Logger( ) : Mile::Manager<Logger>( MString( TEXT( "Logger" ) ) )
        {
        }

        ~Logger( )
        {
            Flush( );
        }

    private:
        std::vector<Mile::Log> Logs;

    };
}