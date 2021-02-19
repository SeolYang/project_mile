#pragma once
#include "Core/CoreMinimal.h"

namespace Mile
{
   class Entity;
   class Context;
   class Transform;
   class MEAPI Component
   {
   public:
      Component(Entity* entity);
      virtual ~Component() { }

      template <typename Ty,
         std::enable_if_t<std::is_base_of_v<Component, Ty>, bool> = true>
      static Ty* Create(Entity* entity)
      {
         Ty* newComp = new Ty(entity);
         newComp->OnCreate();
         return newComp;
      }

      //virtual std::string Serialize( ) const { return ( "\"IsActivated\": " + Mile::BoolSerialize( m_bIsActive )); }
      virtual json Serialize() const { json obj; obj["IsActivated"] = m_bIsActivated; obj["Type"] = WString2String(GetType()); return obj; }
      virtual void DeSerialize(const json& jsonData)
      {
         m_bIsActivated = jsonData["IsActivated"];
      }

      /**
       * @brief   컴포넌트가 활성화 되어있는지 체크합니다. 
       */
      bool IsActivated() const;

      /**
       * @brief   컴포넌트의 활성화 여부를 설정합니다. 
       */
      void SetActive(bool bIsActive);

      virtual void Reset() { }
      virtual void OnCreate() { }
      virtual void Start() { }
      virtual void Update() { OPTICK_EVENT(); }
      virtual void OnEnable() { }
      virtual void OnDisable() { }

      virtual void OnGUI() { }

      virtual String GetType() const { return TEXT("Component"); }

      Entity* GetEntity() const { return m_entity; }
      Context* GetContext() const;
      Transform* GetTransform() const;

      bool IsUpdateEnabled() const { return m_bCanEverUpdate; }

   protected:
      Component() : Component(nullptr)
      {
      }

   protected:
      Entity* m_entity;
      bool    m_bIsActivated;
      bool    m_bCanEverUpdate;

   };
}