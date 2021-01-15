#include "Resource/PlainText.h"
#include "Core/Logger.h"

#define MILE_PLAINTEXT_ERROR_INITIALIZED TEXT("Already initialized PlainText!")
#define MILE_PLAINTEXT_ERROR_FAILED_TO_OPEN_STREAM TEXT("Failed to open stream : ")

namespace Mile
{
   template<>
   bool PlainText<String>::Init()
   {
      if (m_context == nullptr || m_bIsInitialized)
      {
         MELog(m_context, TEXT("PlainText"), ELogType::WARNING, MILE_PLAINTEXT_ERROR_INITIALIZED);
         return false;
      }

      std::wifstream stream(this->m_path);
      if (!stream.is_open())
      {
         MELog(m_context, TEXT("PlainText"), ELogType::WARNING, MILE_PLAINTEXT_ERROR_FAILED_TO_OPEN_STREAM + m_path);
         return false;
      }

      m_data = TEXT("");
      String tmp;
      while (std::getline(stream, tmp))
      {
         m_data += tmp;
         m_data += '\n';
      }

      stream.close();
      return true;

   }

   template<>
   bool PlainText<std::string>::Init()
   {
      if (m_context == nullptr || m_bIsInitialized)
      {
         MELog(m_context, TEXT("PlainText"), ELogType::WARNING, MILE_PLAINTEXT_ERROR_INITIALIZED);
         return false;
      }


      std::ifstream stream(this->m_path);
      if (!stream.is_open())
      {
         MELog(m_context, TEXT("PlainText"), ELogType::WARNING, MILE_PLAINTEXT_ERROR_FAILED_TO_OPEN_STREAM + m_path);
         return false;
      }

      m_data = "";
      std::string tmp;
      while (std::getline(stream, tmp))
      {
         m_data += tmp;
         m_data += '\n';
      }

      stream.close();
      return true;
   }

   template <>
   bool PlainText<String>::SaveTo(const String& filePath)
   {
      std::wofstream stream(filePath, std::ios::trunc);
      stream << m_data;
      stream.close();
      return true;
   }

   template <>
   bool PlainText<std::string>::SaveTo(const String& filePath)
   {
      std::ofstream stream(filePath, std::ios::trunc);
      stream << m_data;
      stream.close();
      return true;
   }
}