#pragma once

#include "SubSystem.h"
#include "Context.h"
#include <forward_list>

namespace Mile
{
   enum class MEAPI ELogType
   {
      MESSAGE,
      DEBUG,
      WARNING,
      FATAL,
      None
   };

   struct MEAPI Log
   {
      Mile::String Category;
      ELogType Type;
      Mile::String Message;
   };

   struct MEAPI LogFilter
   {
      Mile::String Category;
      ELogType Type;
   };

   using LogList = std::forward_list<Log>;
   class MEAPI Logger : public SubSystem
   {
   public:
      Logger( Context* context );
      ~Logger( )
      {
         DeInit( );
      }

      virtual bool Init( ) override;
      virtual void DeInit( ) override;

      void SetLogFolderPath( const Mile::String& folderPath );

      void Logging( const Mile::String& category,
                    ELogType type,
                    const Mile::String& message,
                    bool printConsole = false );

      LogList Filtering( const String& category, ELogType type );
      LogList Filtering( const LogFilter& filter ) { return Filtering( filter.Category, filter.Type ); };

      bool Flush( const String& category = TEXT("all"), ELogType type  = ELogType::None );
      bool Flush( const LogFilter& filter );

      static Mile::String LogTypeToStr( ELogType type )
      {
         switch ( type )
         {
         case ELogType::MESSAGE:
            return TEXT( "Message" );
         case ELogType::DEBUG:
            return TEXT( "Debug" );
         case ELogType::WARNING:
            return TEXT( "Warning" );
         case ELogType::FATAL:
            return TEXT( "Fatal" );
         }

         return TEXT( "None" );
      }

      static Mile::String LogToStr( Log log )
      {
         // [Category][Type][Time] Message
         auto typeStr = Logger::LogTypeToStr( log.Type );
         auto timeStr = NowToWString( );
         auto result = Formatting( TEXT( "[%s][%s][%s] %s" ),
                                   log.Category.c_str( ),
                                   typeStr.c_str( ),
                                   timeStr.c_str( ),
                                   log.Message.c_str( ) );
         return result;
      }

   private:
      Mile::String   m_folderPath;
      LogList        m_logs;
      std::chrono::system_clock::time_point     m_loggingBeginTime;

   };

   static bool MELog( Context* const context,
               const Mile::String& category,
               ELogType type,
               const Mile::String& message,
               bool printConsole = false )
   {
      if ( context == nullptr )
      {
         return false;
      }

      auto logger = context->GetSubSystem<Logger>( );
      if ( logger == nullptr )
      {
         return false;
      }

      logger->Logging( category, type, message, printConsole );

      return true;
   }
}