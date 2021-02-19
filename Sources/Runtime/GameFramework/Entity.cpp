#include "GameFramework/Entity.h"
#include "GameFramework/World.h"
#include "Core/Context.h"
#include "Core/Logger.h"
#include "Component/Component.h"
#include "Component/ComponentRegister.h"
#include "GameFramework/Transform.h"

namespace Mile
{
   DEFINE_LOG_CATEGORY(MileEntity);

   Entity::Entity(World* world, const String& name, const String& tag) :
      m_world(world),
      m_context(nullptr),
      m_bIsActivated(true),
      m_transform(new Transform(this)),
      m_parent(nullptr),
      m_bIsVisibleOnHierarchy(true),
      m_bIsSerializable(true),
      m_bCanEverUpdate(true)
   {
      if (m_world != nullptr)
      {
         m_context = m_world->GetContext();
      }

      SetName(name);
      SetTag(tag);
   }

   Entity::~Entity()
   {
      SafeDelete(m_transform);
      for (auto component : m_components)
      {
         SafeDelete(component);
      }

      for (auto* child : m_children)
      {
         if (child->m_world == nullptr)
         {
            SafeDelete(child);
         }
      }

      m_children.clear();
   }

   json Entity::Serialize() const
   {
      json serialized;
      if (m_bIsSerializable)
      {
         serialized["Name"] = m_nameUTF8;
         serialized["Tag"] = m_tagUTF8;
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
      }

      return serialized;
   }

   void Entity::DeSerialize(const json& jsonData)
   {
      m_nameUTF8 = GetValueSafelyFromJson(jsonData, "Name", std::string("Entity"));
      m_tagUTF8 = GetValueSafelyFromJson(jsonData, "Tag", std::string("None"));
      m_name = String2WString(m_nameUTF8);
      m_tag = String2WString(m_tagUTF8);
      m_bIsActivated = GetValueSafelyFromJson<bool>(jsonData, "IsActivated", true);

      std::vector<json> components = jsonData["Components"];
      for (auto component : components)
      {
         auto foundedType = component.find("Type");
         if (foundedType == component.end())
         {
            ME_LOG(MileEntity, Fatal, TEXT("Can't findout any type information from serialized data!"));
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
      if (IsActivated() && IsUpdateEnabled())
      {
         OPTICK_EVENT();
         for (auto component : m_components)
         {
            if (component->IsActivated() && component->IsUpdateEnabled())
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

   std::vector<Component*>& Entity::GetComponents()
   {
      return m_components;
   }

   std::vector<Component*> Entity::GetComponents() const
   {
      return m_components;
   }
}