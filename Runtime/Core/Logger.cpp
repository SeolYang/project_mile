#include "Logger.h"
#include "Context.h"
#include <memory>
#include <iostream>
#include <string>
#include <cstdio>
#include <chrono>
#include <locale>
#include <fstream>
#include <filesystem>

namespace Mile
{
   Logger::Logger( Context* context ) : 
      m_folderPath( TEXT( "Logs/" ) ),
      SubSystem( context )
   {
   }

   bool Logger::Init( )
   {
      if ( m_context == nullptr || m_bIsInitialized )
      {
         return false;
      }
      
      m_loggingBeginTime = std::chrono::system_clock::now( );
      Logging( TEXT( "Logger" ), ELogType::MESSAGE, TEXT( "Logger initialized!" ), true );
      m_bIsInitialized = true;
      return true;
   }

   void Logger::DeInit( )
   {
      if ( m_bIsInitialized )
      {
         Logging( TEXT( "Logger" ), ELogType::MESSAGE, TEXT( "Logger deinitialized." ), true );
         Flush( );
         SubSystem::DeInit( );
      }
   }

   void Logger::SetLogFolderPath( const Mile::String& folderPath )
   {
      m_folderPath = folderPath;
   }

   void Logger::Logging( const Mile::String& category,
                         ELogType type,
                         const Mile::String& message,
                         bool printConsole )
   {
      auto log = Log{ category, type, message, std::chrono::system_clock::now( ) };
      m_logs.push_front( log );

#ifdef _DEBUG
      if ( printConsole )
      {
         auto formattedLog = LogToStr( log );
         std::wcout << formattedLog << std::endl;
      }
#endif
   }

   LogList Logger::Filtering( const String& category, ELogType type = ELogType::None )
   {
      auto categoryLow = Mile::ToLower( category );
      LogList tempList{ };

      for ( auto log : m_logs )
      {
         bool categoryAgreement = false;
         if ( category == TEXT( "all" ) || category == log.Category )
         {
            categoryAgreement = true;
         }

         bool typeAgreement = false;
         if ( type == ELogType::None || log.Type == type )
         {
            typeAgreement = true;
         }

         if ( categoryAgreement && typeAgreement )
         {
            tempList.push_front( log );
         }
      }

      return tempList;
   }

   bool Logger::Flush( const String& category, ELogType type )
   {
      using namespace std::experimental;
      if ( std::experimental::filesystem::create_directory( m_folderPath ) )
      {
         Logging( TEXT( "Logger" ), ELogType::MESSAGE, TEXT( "Log directory created." ) );
      }

      // Open Stream
      String targetFileName = m_folderPath + 
         Mile::TimeToWString( std::chrono::system_clock::now( ),
                              TEXT( "[%d_%02d_%02d] %02d_%02d_%02d.txt" ) );

      // @TODO: Locale setting
      std::locale::global( std::locale( "Korean" ) );
      std::wofstream logStream{ targetFileName };
      if ( !logStream.is_open( ) )
      {
         std::wcout << TEXT( "Failed to open log stream to " )
            << targetFileName << std::endl;
         return false;
      }

      // Record logging start time
      logStream << TEXT( "#LOG BUILD TIME: " )
         << Mile::TimeToWString( m_loggingBeginTime )
         << std::endl
         << TEXT( "--------------------------------" )
         << std::endl;

      // Record logs
      auto logs = this->Filtering( category, type );
      for ( auto log : logs )
      {
         auto logStr = Logger::LogToStr( log );
         // Write to stream
         logStream << logStr << std::endl;
      }

      logStream << TEXT( "--------------------------------" ) << std::endl;

      // Close Stream
      logStream.flush( );
      logStream.close( );
      return true;
   }

   bool Logger::Flush( const LogFilter& filter )
   {
      return Flush( filter.Category, filter.Type );
   }
}