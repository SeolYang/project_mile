#include "Core/Config.h"
#include "Core/Context.h"
#include "Core/Logger.h"
#include "Resource/ResourceManager.h"
#include "Resource/PlainText.h"

namespace Mile
{
   ConfigSystem::ConfigSystem(Context* context) : SubSystem(context),
      m_nullConfig(TEXT("NULL"), json())
   {
   }

   ConfigSystem::~ConfigSystem()
   {
      DeInit();
   }

   bool ConfigSystem::Init()
   {
      Context* context = GetContext();
      if (SubSystem::Init())
      {

         if (!LoadConfig(TEXT("Engine")))
         {
            MELog(context, TEXT("ConfigSystem"), ELogType::FATAL, TEXT("Failed to load Engine default config. "));
            return false;
         }

         MELog(context, TEXT("ConfigSystem"), ELogType::DEBUG, TEXT("Config System Initialized!"));
         
         SubSystem::InitSucceed();
         return true;
      }

      MELog(context, TEXT("ConfigSystem"), ELogType::WARNING, TEXT("Config System already initialized."));
      return false;
   }

   void ConfigSystem::DeInit()
   {
      if (IsInitialized())
      {
         Context* context = GetContext();
         UnloadAllConfigs();

         SubSystem::DeInit();
         MELog(context, TEXT("ConfigSystem"), ELogType::DEBUG, TEXT("Config System deintialized."));
      }
   }

   bool ConfigSystem::IsExist(const String& configName) const
   {
      for (auto& config : m_configs)
      {
         if (config.first == configName)
         {
            return true;
         }
      }

      return false;
   }

   bool ConfigSystem::LoadConfig(const String& configName)
   {
      if (!IsExist(configName))
      {
         Context* context = GetContext();
         auto resManager = context->GetSubSystem<ResourceManager>();
         auto text = resManager->Load<PlainText<>>(GetPathFromName(configName));

         if (text != nullptr)
         {
            m_configs.push_back(std::make_pair(
               configName,
               json::parse(WString2String(text->GetData()))));

            return true;
         }
      }

      // Failed to load engine config!
      return false;
   }

   bool ConfigSystem::UnloadConfig(const String& configName)
   {
      for (auto itr = m_configs.begin(); itr != m_configs.end(); ++itr)
      {
         if ((*itr).first == configName)
         {
            m_configs.erase(itr);
            return true;
         }
      }

      return false;
   }

   void ConfigSystem::UnloadAllConfigs()
   {
      m_configs.clear();
      m_configs.shrink_to_fit();
   }

   bool ConfigSystem::SaveConfig(const String& configName)
   {
      if (IsExist(configName))
      {
         Context* context = GetContext();
         Config& config = GetConfig(configName);
         auto resManager = context->GetSubSystem<ResourceManager>();
         auto plainText = resManager->Load<PlainText<>>(GetPathFromName(configName));
         auto dumpData = config.second.dump();
         plainText->SetData(String2WString(dumpData));
         return plainText->Save();
      }

      return false;
   }

   void ConfigSystem::SaveAllConfigs()
   {
      Context* context = GetContext();
      auto resManager = context->GetSubSystem<ResourceManager>();
      for (auto& config : m_configs)
      {
         auto plainText = resManager->Load<PlainText<>>(GetPathFromName(config.first));
         auto dumpData = config.second.dump();
         plainText->SetData(String2WString(dumpData));
         plainText->Save();
      }
   }

   Config& ConfigSystem::GetConfig(const String& configName)
   {
      for (auto& config : m_configs)
      {
         if (config.first == configName)
         {
            return config;
         }
      }

      return m_nullConfig;
   }

   Config ConfigSystem::GetConfig(const String& configName) const
   {
      for (auto& config : m_configs)
      {
         if (config.first == configName)
         {
            return config;
         }
      }

      return m_nullConfig;
   }
}