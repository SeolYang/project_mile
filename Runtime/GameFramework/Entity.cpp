#include "GameFramework/Entity.h"
#include "GameFramework/World.h"
#include "Core/Context.h"
#include "Core/Logger.h"
#include "Component/Component.h"
#include "Component/ComponentRegister.h"

namespace Mile
{
   Entity::Entity(World* world, const String& name, const String& tag) :
      m_world(world),
      m_context(nullptr),
      m_name(name),
      m_tag(tag),
      m_bIsActivated(true),
      m_transform(new Transform(this)),
      m_parent(nullptr)
   {
      if (m_world != nullptr)
      {
         m_context = m_world->GetContext();
      }
   }

   Entity::~Entity()
   {
      SafeDelete(m_transform);
      for (auto component : m_components)
      {
         SafeDelete(component);
      }

      m_children.clear();
   }

   json Entity::Serialize() const
   {
      json serialized;
      serialized["Name"] = WString2String(m_name);
      serialized["Tag"] = WString2String(m_tag);
      serialized["IsActivated"] = m_bIsActivated;
      serialized["Transform"] = m_transform->Serialize();

      std::vector<json> components;
      for (auto comp : m_components)
      {
         components.push_back(comp->Serialize());
      }
      serialized["Components"] = components;

      std::vector<json> children;
      for (auto child : m_children)
      {
         children.push_back(child->Serialize());
      }
      serialized["Children"] = children;

      return serialized;
   }

   void Entity::DeSerialize(const json& jsonData)
   {
      m_name = String2WString(jsonData["Name"]);
      m_tag = String2WString(jsonData["Tag"]);
      m_bIsActivated = jsonData["IsActivated"];

      std::vector<json> components = jsonData["Components"];
      for (auto component : components)
      {
         auto foundedType = component.find("Type");
         if (foundedType == component.end())
         {
            MELog(this->GetContext(),
               TEXT("Entity"), ELogType::FATAL,
               TEXT("Can't findout any type information from serialized data!"));
         }

         std::string type = (*foundedType);
         Component* compInst = ComponentRegister::GetInstance().Acquire(type, this);
         compInst->DeSerialize(component);
         m_components.push_back(compInst);
      }

      std::vector<json> children = jsonData["Children"];
      for (auto child : children)
      {
         Entity* tempChild = m_world->CreateEntity(TEXT(""));
         tempChild->DeSerialize(child);
         this->AttachChild(tempChild);
      }

      m_transform->DeSerialize(jsonData["Transform"]);
   }

   bool Entity::Init()
   {
      return true;
   }

   void Entity::SetActive(bool bIsActive)
   {
      if (m_bIsActivated != bIsActive)
      {
         m_bIsActivated = bIsActive;
         if (m_bIsActivated)
         {
            OnEnable();
         }
         else
         {
            OnDisable();
         }
      }
   }

   bool Entity::AttachChild(Entity* child)
   {
      if (child != nullptr)
      {
         if (child != this && child->m_parent != this)
         {
            Entity* oldParent = child->m_parent;
            if (oldParent != nullptr)
            {
               if (!oldParent->DetachChild(child))
               {
                  return false;
               }
            }

            child->m_parent = this;
            child->GetTransform()->SetParent(m_transform);
            m_children.push_back(child);
            return true;
         }
      }

      return false;
   }

   bool Entity::DetachChild(Entity* child)
   {
      bool bIsValidChild = child != nullptr && child != this;
      if (bIsValidChild)
      {
         if (child->m_parent == this)
         {
            m_children.erase(std::find(m_children.begin(), m_children.end(), child));
            child->GetTransform()->SetParent(nullptr);
            return true;
         }
      }

      return false;
   }

   void Entity::SetTransform(const Transform& transform)
   {
      (*m_transform) = transform;
   }

   Entity* Entity::GetChildByName(const String& name)
   {
      for (auto child : m_children)
      {
         if (child->GetName() == name)
         {
            return child;
         }
      }

      return nullptr;
   }

   void Entity::Start()
   {
      for (auto component : m_components)
      {
         component->Start();
      }
   }

   void Entity::Update()
   {
      if (m_bIsActivated)
      {
         for (auto component : m_components)
         {
            if (component->IsActivated())
            {
               component->Update();
            }
         }

         for (auto child : m_children)
         {
            child->Update();
         }
      }
   }

   void Entity::OnEnable()
   {
      for (auto component : m_components)
      {
         if (component->IsActivated())
         {
            component->OnEnable();
         }
      }
   }

   void Entity::OnDisable()
   {
      for (auto component : m_components)
      {
         if (component->IsActivated())
         {
            component->OnDisable();
         }
      }
   }
}