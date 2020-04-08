#pragma once
#include "Core/SubSystem.h"
#include "Component/Component.h"

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
      std::vector<ComponentType*> GetComponentsFromEntities(bool onlyActivatedOwner = false, bool onlyActivated = false) const
      {
         std::vector<ComponentType*> foundComponents;
         for (Entity* entity : m_entities)
         {
            if (entity != nullptr)
            {
               ComponentType* foundComponent = nullptr;
               if ((onlyActivatedOwner && entity->IsActivated()) || (!onlyActivatedOwner))
               {
                  /**
                  *  월드에서는 자식 entity/부모 entity 상관없이 월드상에 존재할 수 있는 모든 entity들을 가지고 있기때문에.
                  *  각각의 entity에 대해서 개별적으로 GetComponent를 이용해서 컴포넌트를 가져온다.
                  */
                  foundComponent = entity->GetComponent<ComponentType>();
               }

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

      bool LoadFrom(const String& filePath);
      bool SaveTo(const String& filePath);
      bool Save();

   private:
      std::vector<Entity*> m_entities;
      PlainText<std::string>* m_loadedData;

   };
}
