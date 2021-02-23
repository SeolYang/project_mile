#pragma once
#include "Resource/Resource.h"
#include "Core/Logger.h"

namespace Mile
{
   DECLARE_LOG_CATEGORY_EXTERN(MilePlainText, Log);

   template <typename StrType = String>
   class MEAPI PlainText : public Resource
   {
   public:
      PlainText(ResourceManager* resMng) : 
         Resource(resMng, ResourceType::PlainText)
      {
      }

      virtual bool Init(const String& filePath) override;
      virtual bool SaveTo(const String& filePath) override;

      StrType GetData() const { return m_data; }

      void SetData(const StrType& newData)
      {
         m_data = newData;
      }

   private:
      StrType m_data;

   };
}