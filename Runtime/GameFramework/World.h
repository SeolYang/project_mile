#pragma once
#include "Core/SubSystem.h"
#include "Component/Component.h"
#include "GameFramework/Entity.h"

#define MILE_WORLD_LOG_CATEGORY TEXT("World")

namespace Mile
{
   template<typename Ty>
   class PlainText;
   class Entity;
   class MEAPI World : public SubSystem
   {
   public:
      World(Context* context);
      virtual ~World();

      //std::string Serialize( ) const;
      json Serialize() const;
      void DeSerialize(const json& jsonData);

      /**
      * @brief    World를 초기화합니다.
      */
      virtual bool Init() override;
      virtual void DeInit() override;

      /**
      * @brief    World에 생성되어있는 Entity들의 Start 함수를 호출합니다.
      */
      void Start();

      /**
      * @brief    World에 생성되어있는 Entity들을 Update 합니다.
      */
      void Update();

      /**
      * @brief    World에 새로운 Entity를 생성합니다.
      * @return   생성된 Entity
      */
      Entity* CreateEntity(const String& name);

      /**
       * @brief   World에서 지정된 Entity를 찾아 삭제합니다
       * @return  삭제에 성공했는지
       */
      bool DestroyEntity(Entity* target);

      /**
       * @breif   이 World에 속해있는 Entity인지 확인합니다.
       */
      bool CheckEntityValidation(Entity* target) const;

      /**
      * @brief    World에 생성되어있는 Entitiy중 주어진 이름을 가진 Entity를 반환합니다.
      *           ( 단 같은 이름을 가지는 Entity중 제일 먼저 추가된 Entity를 반환함. )
      * @param    해당하는 이름을 가지는 Entity
      */
      Entity* GetEntityByName(const String& name) const;

      /**
      * @brief    월드에 생성되어있는 모든 Entity들의 배열을 반환합니다.
      * @return   Entity 배열
      */
      std::vector<Entity*> GetEntities() const;

      /**
      * @brief    월드에 생성되어있는 Entity 중 Root Entitiy들의 배열을 반환합니다.
      */
      std::vector<Entity*> GetRootEntities() const;

      /**
       * @brief   월드에 생성되어있는 모든 Entity로 부터 주어진 컴포넌트 타입의 컴포넌트들을 모두 반환합니다.
       */
      template <typename ComponentType, 
         std::enable_if_t<std::is_base_of_v<Component, ComponentType>, bool> = true>
      std::vector<ComponentType*> GetComponentsFromEntities(bool onlyActivated = true) const
      {
         std::vector<ComponentType*> foundComponents;
         for (Entity* entity : m_entities)
         {
            if (entity != nullptr)
            {
               ComponentType* foundComponent = nullptr;
               foundComponent = entity->GetComponent<ComponentType>();

               if (foundComponent != nullptr)
               {
                  if ((onlyActivated && foundComponent->IsActivated()) || (!onlyActivated))
                  {
                     foundComponents.push_back(foundComponent);
                  }
               }
            }
         }

         return foundComponents;
      }

      bool LoadFrom(const String& filePath, bool bClearWorld = true);
      bool SaveTo(const String& filePath);
      bool Save();

      bool IsSaved() const { return (m_loadedData != nullptr); }

      /**
       * @breif   월드에 생성되어 있는 모든 Entity를 삭제합니다.
       */
      void Clear();

      String GetWorldName() const { return m_name; }

   private:
      String m_name;
      std::vector<Entity*> m_entities;
      PlainText<std::string>* m_loadedData;

   };
}
