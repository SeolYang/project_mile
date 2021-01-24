#include "Core/Logger.h"
#include "Core/Engine.h"
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
   Logger::Logger(Context* context) :
      m_folderPath(TEXT("Logs/")),
      SubSystem(context)
   {
   }

   bool Logger::Init()
   {
      if (SubSystem::Init())
      {
         m_loggingBeginTime = std::chrono::system_clock::now();

         SubSystem::InitSucceed();
         return true;
      }

      return false;
   }

   void Logger::DeInit()
   {
      if (IsInitialized())
      {
         Flush();

         SubSystem::DeInit();
      }
   }

   void Logger::SetLogFolderPath(const Mile::String& folderPath)
   {
      m_folderPath = folderPath;
   }

   void Logger::Logging(const LogCategoryBase& category,
      ELogVerbosity verbosity,
      const Mile::String& message)
   {
      ELogVerbosity finalVerbosity = verbosity;
      if (finalVerbosity == ELogVerbosity::All)
      {
         finalVerbosity = category.DefaultVerbosity;
      }

      auto log = MLog{ category, finalVerbosity, message, std::chrono::system_clock::now() };
      m_logs.push_front(log);


#ifdef _DEBUG
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY);
      std::wcout << TEXT("[") << TimeToWString(log.Time) << TEXT("]");
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), VerbosityToConsoleColor(finalVerbosity));
      std::wcout << TEXT("[") << log.Category.Name << TEXT("]");
      if (log.Verbosity != ELogVerbosity::Log)
      {
         std::wcout << TEXT("[") << VerbosityToString(log.Verbosity) << TEXT("]");
      }
      std::wcout << TEXT(" ") << log.Message << std::endl;
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), VerbosityToConsoleColor(ELogVerbosity::All));
#endif
   }

   LogList Logger::Filtering(const LogCategoryBase& category, ELogVerbosity verbosity)
   {
      LogList tempList{ };

      for (auto log : m_logs)
      {
         bool bCheckCategory = category.Name == log.Category.Name;
         bool bCheckVerbosity = 
            verbosity == ELogVerbosity::All ||
            log.Verbosity == verbosity;
         if (bCheckCategory && bCheckVerbosity)
         {
            tempList.push_front(log);
         }
      }

      return tempList;
   }

   void Logger::GlobalLogging(const LogCategoryBase& category, ELogVerbosity verbosity, const Mile::String& message)
   {
      auto globalLogger = Engine::GetLogger();
      if (globalLogger != nullptr)
      {
         globalLogger->Logging(category, verbosity, message);
      }
   }

   bool Logger::Flush(const LogList& list)
   {
      std::filesystem::create_directory(m_folderPath);

      // Open Stream
      String targetFileName = m_folderPath +
         Mile::TimeToWString(std::chrono::system_clock::now(),
            TEXT("[%d_%02d_%02d] %02d_%02d_%02d.mlog"));

      // @TODO: Locale setting
      std::locale::global(std::locale("Korean"));
      std::wofstream logStream{ targetFileName };
      if (!logStream.is_open())
      {
         std::wcout << TEXT("Failed to open log stream to ")
            << targetFileName << std::endl;
         return false;
      }

      // Record logging start time
      logStream << TEXT("#LOG BUILD TIME: ")
         << Mile::TimeToWString(m_loggingBeginTime)
         << std::endl
         << TEXT("--------------------------------")
         << std::endl;

      // Record logs
      for (auto log : list)
      {
         auto logStr = Logger::LogToStr(log);
         // Write to stream
         logStream << logStr << std::endl;
      }

      logStream << TEXT("--------------------------------") << std::endl;

      // Close Stream
      logStream.flush();
      logStream.close();
   }

   bool Logger::Flush(const LogCategoryBase& category, ELogVerbosity verbosity)
   {
      return Flush(Filtering(category, verbosity));
   }

   bool Logger::Flush()
   {
      return Flush(m_logs);
   }
}