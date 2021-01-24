#pragma once
#include "Core/CoreMinimal.h"
#include "Component/Component.h"
#include "GameFramework/Transform.h"

#define DEFAULT_ENTITY_NAME TEXT("Entity")
#define DEFAULT_ENTITY_TAG TEXT("Default")

namespace Mile
{
   DECLARE_LOG_CATEGORY_EXTERN(MileEntity, Log);

   class Context;
   class World;
   class Component;
   class MEAPI Entity
   {
      friend World;
      friend class ModelLoader;

   private:
      Entity(World* world, const String& name = DEFAULT_ENTITY_NAME, const String& tag = DEFAULT_ENTITY_TAG);

   public:
      ~Entity();

      json Serialize() const;
      // Null world entitiy must not deserialized.
      void DeSerialize(const json& jsonData);

      /**
      * @brief    Entity를 초기화 합니다. ( 이 시점에서 Entity의 Transform 이 설정됩니다. )
      * @param    Entity에 적용할 Transform 컴포넌트
      */
      bool Init();

      /**
      * @brief    템플릿 파라미터로 주어진 형식의 컴포넌트를 추가합니다.
      * @return   추가된 컴포넌트
      */
      template <typename Ty> Ty* AddComponent();

      /**
      * @brief    주어진 컴포넌트가 Entity에 등록되어있다면 제거합니다.
      * @param    Entity에 등록되어있는 컴포넌트
      */
      template <typename Ty> void RemoveComponent(Ty* target);

      /**
      * @brief    템플릿 파라미터로 주어진 형식의 모든 컴포넌트들을 제거합니다. ( 단 메모리의 할당해제는 일어나지 않습니다. )
      */
      template <typename Ty> void RemoveComponents();

      /**
      * @brief    템플릿 파라미터로 주어진 형식의 컴포넌트중 가장 먼저 추가된 컴포넌트를 반환합니다.
      * @return   가장 먼저 추가된 해당 형식의 컴포넌트
      */
      template <typename Ty> Ty* GetComponent() const;

      /**
      * @brief    Entitiy에 추가되어있는 모든 컴포넌트들을 반환합니다.
      * @return   Entitiy에 추가되어있는 컴포넌트 들의 배열
      */
      std::vector<Component*>& GetComponents();
      std::vector<Component*> GetComponents() const;

      /**
      * @brief    템플릿 파라미터로 주어진 형식의 컴포넌트가 Entity에 추가되어있는지 확인합니다.
      * @return   해당 형식의 컴포넌트가 추가 여부
      */
      template <typename Ty> bool HasComponent() const;

      /**
      * @brief    Entity가 활성화 되어있는지 확인합니다.
      * @return   Entity의 활성화 여부
      */
      bool IsActivated() const { return (m_parent != nullptr) ? m_bIsActivated && m_parent->IsActivated() : m_bIsActivated; }

      /**
      * @brief    Entity를 활성화 시키거나 비 활성화 시킵니다.
      * @param    Entity의 활성화 여부
      */
      void SetActive(bool bIsActive);

      /**
      * @brief    Entity에 등록되어있는 Transform 컴포넌트를 반환합니다.
      * @return   Entity의 Transform 컴포넌트
      */
      Transform* GetTransform() { return m_transform; }

      void SetTransform(const Transform& transform);

      /**
      * @brief    Entity에 설정되어있는 이름을 반환합니다.
      * @return   Entity의 이름
      */
      String GetName() const { return m_name; }

      /**
      * @brief    Entity에 새로운 이름을 설정해줍니다.
      * @param    Entity에 설정해줄 새로운 이름
      */
      void SetName(const String& name) { m_name = name; }

      String GetTag() const { return m_tag; }
      void SetTag(const String& tag) { m_tag = tag; }

      bool AttachChild(Entity* child);
      bool DetachChild(Entity* child);

      Entity* GetParent() { return m_parent; }
      Entity* GetChildByName(const String& name);
      bool HasParent() { return m_parent != nullptr; }

      bool IsVisibleOnHierarchy() const { return m_bIsVisibleOnHierarchy; }
      void SetVisibleOnHierarchy(bool bIsVisible) { m_bIsVisibleOnHierarchy = bIsVisible; }

      bool IsSerializationDisabled() const { return m_bIsSerializable; }
      void SetSerializable(bool bIsSerializable) { m_bIsSerializable = bIsSerializable; }

      std::vector<Entity*> GetChildren() const { return m_children; }
      std::vector<Entity*> GetChildren() { return m_children; }

      void Start();
      void Update();
      void OnEnable();
      void OnDisable();

      World* GetWorld() const { return m_world; }
      Context* GetContext() const { return m_context; }

   private:
      bool     m_bIsActivated;
      Context* m_context;
      World*   m_world;
      Entity*  m_parent;
      Transform* m_transform;
      std::vector<Component*> m_components;
      std::vector<Entity*>    m_children;
      String   m_name;
      String   m_tag;

      bool  m_bIsVisibleOnHierarchy;
      bool  m_bIsSerializable;

   };

   template <typename Ty>
   Ty* Entity::AddComponent()
   {
      Component* component = new Ty(this);
      m_components.push_back(component);

      component->Reset();
      component->SetActive(true);

      return static_cast<Ty*>(component);
   }

   template <typename Ty>
   void Entity::RemoveComponent(Ty* target)
   {
      if (target != nullptr)
      {
         for (auto itr = m_components.begin(); itr != m_components.end(); ++itr)
         {
            SafeDelete(target);
            m_components.erase(itr);
            break;
         }
      }
   }

   template <typename Ty>
   void Entity::RemoveComponents()
   {
      for (auto itr = m_components.begin(); itr != m_components.end(); ++itr)
      {
         Component* foundComponent = *itr;
         if (typeid(Ty) == typeid (*(foundComponent)))
         {
            SafeDelete(foundComponent);
            m_components.erase(itr);
         }
      }
   }

   template <typename Ty>
   Ty* Entity::GetComponent() const
   {
      for (auto component : m_components)
      {
         if (typeid(Ty) == typeid(*component))
         {
            return reinterpret_cast<Ty*>(component);
         }
      }

      return nullptr;
   }

   template <typename Ty>
   bool Entity::HasComponent() const
   {
      return ((GetComponent<Ty>()) != nullptr);
   }
}
