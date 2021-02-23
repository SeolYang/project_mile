#include "Resource/PlainText.h"
#include "Core/Logger.h"

#define MILE_PLAINTEXT_ERROR_INITIALIZED TEXT("Already initialized PlainText!")
#define MILE_PLAINTEXT_ERROR_FAILED_TO_OPEN_STREAM TEXT("Failed to open stream : ")

namespace Mile
{
   DEFINE_LOG_CATEGORY(MilePlainText);

   template<>
   bool PlainText<String>::Init(const String& filePath)
   {
      if (Resource::Init(filePath))
      {
         std::wifstream stream(this->m_path);
         if (!stream.is_open())
         {
            ME_LOG(MilePlainText, Warning, MILE_PLAINTEXT_ERROR_FAILED_TO_OPEN_STREAM + m_path);
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
         SucceedInit();
         return true;
      }

      ME_LOG(MilePlainText, Warning, MILE_PLAINTEXT_ERROR_INITIALIZED);
      return false;
   }

   template<>
   bool PlainText<std::string>::Init(const String& filePath)
   {
      if (Resource::Init(filePath))
      {
         std::ifstream stream(this->m_path);
         if (!stream.is_open())
         {
            ME_LOG(MilePlainText, Warning, MILE_PLAINTEXT_ERROR_FAILED_TO_OPEN_STREAM + m_path);
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
         SucceedInit();
         return true;
      }

      ME_LOG(MilePlainText, Warning, MILE_PLAINTEXT_ERROR_INITIALIZED);
      return false;
   }

   template <>
   bool PlainText<String>::SaveTo(const String& filePath)
   {
      if (Resource::SaveTo(filePath))
      {
         std::wofstream stream(filePath, std::ios::trunc);
         stream << m_data;
         stream.close();
         return true;
      }

      return false;
   }

   template <>
   bool PlainText<std::string>::SaveTo(const String& filePath)
   {
      if (Resource::SaveTo(filePath))
      {
         std::ofstream stream(filePath, std::ios::trunc);
         stream << m_data;
         stream.close();
         return true;
      }

      return false;
   }
}