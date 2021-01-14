#include "GameFramework/World.h"
#include "GameFramework/Entity.h"
#include "Core/Context.h"
#include "Core/Logger.h"
#include "Core/Config.h"
#include "Resource/ResourceManager.h"
#include "Resource/PlainText.h"
#include <iostream>

namespace Mile
{
   World::World(Context* context) :
      m_loadedData(nullptr),
      SubSystem(context)
   {
   }

   World::~World()
   {
      DeInit();
   }

   json World::Serialize() const
   {
      json serialized;
      std::vector<json> serializedEntities;
      for (auto entity : m_entities)
      {
         if (!entity->HasParent())
         {
            serializedEntities.push_back(entity->Serialize());
         }
      }

      serialized["Entities"] = serializedEntities;
      return serialized;
   }

   void World::DeSerialize(const json& jsonData)
   {
      if (jsonData.find("Entities") != jsonData.end())
      {
         std::vector<json> entities = jsonData["Entities"];
         for (auto entity : entities)
         {
            Entity* temp = CreateEntity(TEXT(""));
            temp->DeSerialize(entity);
         }
      }
   }

   bool World::Init()
   {
      Context* context = GetContext();
      if (SubSystem::Init())
      {
         auto configSys = context->GetSubSystem<ConfigSystem>();
         auto engineConfig = configSys->GetConfig(TEXT("Engine"));
         String defaultPath = String2WString(engineConfig.second["World"]);

         auto loaded = LoadFrom(defaultPath);
         if (!loaded)
         {
            MELog(context, TEXT("World"), ELogType::FATAL, TEXT("Failed to load world from ") + defaultPath, true);
            return false;
         }

         MELog(context, TEXT("World"), ELogType::MESSAGE, TEXT("World Initialized!"), true);
         SubSystem::InitSucceed();
         return true;
      }

      MELog(context, TEXT("World"), ELogType::FATAL, TEXT("Failed to initialize world."), true);
      return false;
   }

   void World::DeInit()
   {
      if (IsInitialized())
      {
         Clear();

         SubSystem::DeInit();
         MELog(GetContext(), TEXT("World"), ELogType::MESSAGE, TEXT("World deinitialized."), true);
      }
   }

   void World::Start()
   {
      for (auto entity : m_entities)
      {
         entity->Start();
      }
   }

   void World::Update()
   {
      for (auto entity : m_entities)
      {
         entity->Update();
      }
   }

   Entity* World::CreateEntity(const String& name)
   {
      auto newEntity = new Entity(this, name);
      m_entities.push_back(newEntity);

      return newEntity;
   }

   Entity* World::GetEntityByName(const String& name) const
   {
      for (const auto& entity : m_entities)
      {
         if (name == (entity)->GetName())
         {
            return entity;
         }
      }

      return nullptr;
   }

   std::vector<Entity*> World::GetEntities() const
   {
      return m_entities;
   }

   std::vector<Entity*> World::GetRootEntities() const
   {
      std::vector<Entity*> rootEntities{ };
      for (auto entity : m_entities)
      {
         if (entity->GetParent() == nullptr)
         {
            rootEntities.push_back(entity);
         }
      }

      return rootEntities;
   }

   bool World::LoadFrom(const String& filePath, bool bClearWorld)
   {
      Context* context = GetContext();
      auto resMng = context->GetSubSystem<ResourceManager>();
      auto res = resMng->Create<PlainText<std::string>>(filePath);
      if (res != nullptr)
      {
         if (bClearWorld)
         {
            Clear();
         }

         this->m_loadedData = res;
         this->DeSerialize(json::parse(res->GetData().empty() ? "{}" : res->GetData()));
         MELog(context, TEXT("World"), ELogType::MESSAGE, TEXT("World loaded. : ") + filePath, true);
         return true;
      }

      MELog(context, TEXT("World"), ELogType::FATAL, TEXT("World load failed. : ") + filePath, true);
      return false;
   }

   bool World::SaveTo(const String& filePath)
   {
      Context* context = GetContext();
      auto resMng = context->GetSubSystem<ResourceManager>();
      auto res = resMng->GetByPath<PlainText<std::string>>(filePath);

      if (res == nullptr)
      {
         res = resMng->Create<PlainText<std::string>>(filePath);
      }

      if (res != nullptr)
      {
         res->SetData(this->Serialize().dump());
         if (res->Save())
         {
            MELog(context, TEXT("World"), ELogType::MESSAGE, TEXT("World data saved. : ") + res->GetPath(), true);
            return true;
         }
      }

      return false;
   }

   bool World::Save()
   {
      if (m_loadedData != nullptr)
      {
         m_loadedData->SetData(this->Serialize().dump());
         if (m_loadedData->Save())
         {
            MELog(GetContext(), TEXT("World"), ELogType::MESSAGE, TEXT("World data saved. : ") + m_loadedData->GetPath(), true);
            return true;
         }
      }

      return SaveTo(TEXT("NewWorld.world"));
   }

   void World::Clear()
   {
      for (auto entity : m_entities)
      {
         SafeDelete(entity);
      }

      m_entities.clear();
   }
}