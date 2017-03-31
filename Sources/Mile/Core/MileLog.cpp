#include "MileLog.h"

namespace Mile
{
    void Logger::Log( LogType Type, const MString& Message )
    {
        std::chrono::system_clock::time_point Now = std::chrono::system_clock::now( );
        std::time_t NowTime = std::chrono::system_clock::to_time_t( Now );
        MString Time{ _wctime( &NowTime ) };

        Logs.push_back( Mile::Log( Type, Message, Time ) );
    }

    void Logger::Log( const MString& Message )
    {
        Log( LogType::Log, Message );
    }

    void Logger::Debug( const MString& Message )
    {
        Log( LogType::Debug, Message );
    }

    void Logger::Warning( const MString& Message )
    {
        Log( LogType::Warning, Message );
    }

    void Logger::Error( const MString& Message )
    {
        Log( LogType::Error, Message );
    }

    void Logger::Critical( const MString& Message )
    {
        Log( LogType::Critical, Message );
    }

    void Logger::Clear( )
    {
        Logs.clear( );
    }

    void Logger::Flush( )
    {
        Clear( );
    }

    Log::operator MString( ) const
    {
        MString LogTimePrefix;
        MString LogTypePrefix = MString( TEXT( "\t: Log\t:" ) );

        switch ( Type )
        {
        case LogType::Debug:
            LogTypePrefix = MString( TEXT( "\t: Debug\t:" ) );
            break;

        case LogType::Warning:
            LogTypePrefix = MString( TEXT( "\t: Warning\t:" ) );
            break;

        case LogType::Error:
            LogTypePrefix = MString( TEXT( "\t: Error\t:" ) );
            break;

        case LogType::Critical:
            LogTypePrefix = MString( TEXT( "\t: Critical\t:" ) );
            break;
        }

        return ( LogTimePrefix + LogTypePrefix + Message );
    }
}