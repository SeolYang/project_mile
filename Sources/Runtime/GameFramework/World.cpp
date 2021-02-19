#include "GameFramework/World.h"
#include "Core/Context.h"
#include "Core/Config.h"
#include "Resource/ResourceManager.h"
#include "Resource/PlainText.h"

namespace Mile
{
   DEFINE_LOG_CATEGORY(MileWorld);

   World::World(Context* context) :
      m_name(TEXT("Untitled")),
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

      serialized["Entities"] = std::move(serializedEntities);
      return serialized;
   }

   void World::DeSerialize(const json& jsonData)
   {
      if (jsonData.find("Entities") != jsonData.end())
      {
         const std::vector<json>& entities = jsonData["Entities"];
         for (const auto& entity : entities)
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
         ME_LOG(MileWorld, Log, TEXT("World Initialized."));
         SubSystem::InitSucceed();

         return true;
      }

      ME_LOG(MileWorld, Fatal, TEXT("Failed to initialize world."));
      return false;
   }

   void World::DeInit()
   {
      if (IsInitialized())
      {
         Clear();
         SubSystem::DeInit();
         ME_LOG(MileWorld, Log, TEXT("World deinitialized."));
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
      OPTICK_EVENT();
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

   bool World::DestroyEntity(Entity* target)
   {
      for (auto itr = m_entities.begin(); itr != m_entities.end(); ++itr)
      {
         if (target == (*itr))
         {
            ME_LOG(MileWorld, Log, String(TEXT("Destroy entity : ")) + target->GetName());
            m_entities.erase(itr);
            for (auto child : target->GetChildren())
            {
               DestroyEntity(child);
            }

            SafeDelete(target);
            return true;
         }
      }

      return false;
   }

   bool World::CheckEntityValidation(Entity* target) const
   {
      for (Entity* entity : m_entities)
      {
         if (entity == target)
         {
            return true;
         }
      }

      return false;
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
         this->m_name = m_loadedData->GetName();
         this->DeSerialize(json::parse(res->GetData().empty() ? "{}" : res->GetData()));
         ME_LOG(MileWorld, Log, TEXT("World loaded. : ") + filePath);
         return true;
      }

      ME_LOG(MileWorld, Fatal, TEXT("World load failed. : ") + filePath);
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
            m_loadedData = res;
            m_name = m_loadedData->GetName();
            ME_LOG(MileWorld, Log, TEXT("World data saved. : ") + res->GetPath());
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
            ME_LOG(MileWorld, Log, TEXT("World data saved. : ") + m_loadedData->GetPath());
            return true;
         }
      }

      return false;
   }

   void World::Clear()
   {
      for (auto entity : m_entities)
      {
         SafeDelete(entity);
      }

      m_entities.clear();
      m_loadedData = nullptr;

      m_name = TEXT("Untitled");

      ME_LOG(MileWorld, Log, TEXT("World has been cleared."));
   }
}