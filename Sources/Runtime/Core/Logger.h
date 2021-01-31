#pragma once
#include "Core/SubSystem.h"
#include <forward_list>

namespace Mile
{
   /**
    * @brief   로그의 상세정도를 나타냅니다.
    *  Fatal   Fatal 레벨은 로거가 비활성화 되어있더라도 콘솔과 로그 파일에 무조건 기록 됩니다.
    *  Error   Error 레벨은 콘솔과 로그 파일에 출력 됩니다.
    *  Warning Warning 레벨은 콘솔과 로그 파일에 출력 됩니다.
    *  Display Display 레벨의 로그는 콘솔과 로그 파일에 출력 됩니다.
    *  Log     로그는 로그 파일에는 기록되지만 인게임 콘솔에는 출력 되지 않습니다.
    */
   enum MEAPI ELogVerbosity : UINT32
   {
      All = 0,
      Fatal,
      Error,
      Warning,
      Display,
      Log,
      EnumSize
   };

   struct MEAPI LogCategoryBase
   {
   public:
      LogCategoryBase(const Mile::String& name, ELogVerbosity defaultVerbosity) :
         Name(name),
         DefaultVerbosity(defaultVerbosity)
      {
      }

   public:
      Mile::String Name = TEXT("Unknown");
      ELogVerbosity DefaultVerbosity = ELogVerbosity::Log;

   };

   struct MEAPI MLog
   {
      LogCategoryBase Category;
      ELogVerbosity Verbosity = ELogVerbosity::All;
      Mile::String Message = TEXT("");
      std::chrono::system_clock::time_point Time = std::chrono::system_clock::time_point();
   };

   using LogList = std::forward_list<MLog>;
   class MEAPI Logger : public SubSystem
   {
   public:
      Logger(Context* context);
      virtual ~Logger();

      virtual bool Init() override;
      virtual void DeInit() override;

      void SetLogFolderPath(const Mile::String& folderPath);

      void Logging(const LogCategoryBase& category,
         ELogVerbosity verbosity = ELogVerbosity::All,
         const Mile::String& message = TEXT(""));

      LogList Filtering(const LogCategoryBase& category, ELogVerbosity verbosity) const;

      /** Flush 함수들은 Thread-safe 하지 않습니다. */
      bool Flush();
      /** Flush 함수들은 Thread-safe 하지 않습니다. */
      bool Flush(const LogCategoryBase& category, ELogVerbosity verbosity = ELogVerbosity::All);

      static Mile::String VerbosityToString(ELogVerbosity verbosity)
      {
         switch (verbosity)
         {
         default:
         case ELogVerbosity::All:
            return TEXT("All");
         case ELogVerbosity::Fatal:
            return TEXT("Fatal");
         case ELogVerbosity::Error:
            return TEXT("Error");
         case ELogVerbosity::Log:
            return TEXT("Log");
         case ELogVerbosity::Warning:
            return TEXT("Warning");
         }
      }

      static int VerbosityToConsoleColor(ELogVerbosity type)
      {
         switch (type)
         {
         default:
            return FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;
         case ELogVerbosity::Log:
            return FOREGROUND_GREEN | FOREGROUND_INTENSITY;
         case ELogVerbosity::Warning:
            return FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;
         case ELogVerbosity::Fatal:
         case ELogVerbosity::Error:
            return FOREGROUND_RED | FOREGROUND_INTENSITY;
         }
      }

      static Mile::String LogToStr(const MLog& log)
      {
         // [Category][Type][Time] Message
         auto typeStr = Logger::VerbosityToString(log.Verbosity);
         auto timeStr = TimeToWString(log.Time);
         auto result = Formatting(TEXT("[%s][%s][%s] %s"),
            timeStr.c_str(),
            log.Category.Name.c_str(),
            typeStr.c_str(),
            log.Message.c_str());
         return result;
      }

      static void GlobalLogging(const LogCategoryBase& category,
         ELogVerbosity verbosity = ELogVerbosity::All,
         const Mile::String& message = TEXT(""));

   private:
      bool Flush(const LogList& list);

   private:
      mutable std::mutex m_mutex;
      String   m_folderPath;
      LogList  m_logs;
      std::chrono::system_clock::time_point  m_loggingBeginTime;

   };

#define DECLARE_LOG_CATEGORY_EXTERN(CategoryName, DefaultVerbosity) \
   namespace DefinedLogCategoryType { \
      struct CategoryName##Type : public LogCategoryBase { \
      public: \
         CategoryName##Type() : LogCategoryBase(TEXT(#CategoryName), DefaultVerbosity) \
         { \
         } \
      }; \
   } \
   extern DefinedLogCategoryType::CategoryName##Type CategoryName

#define DEFINE_LOG_CATEGORY(CategoryName) DefinedLogCategoryType::CategoryName##Type CategoryName

/* Mile Engine Log **/
#define ME_LOG(CategoryName, Verbosity, Message, ...) Logger::GlobalLogging(CategoryName, Verbosity, Format(Message, __VA_ARGS__))

DECLARE_LOG_CATEGORY_EXTERN(MileLogger, Log);
}